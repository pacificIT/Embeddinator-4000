﻿using CppSharp.AST;
using CppSharp.AST.Extensions;
using System.Linq;

namespace MonoManagedToNative.Generators
{
    public class CSources : CTemplate
    {
        public CSources(Driver driver, TranslationUnit unit) : base(driver, unit)
        {
        }

        public override string FileExtension
        {
            get { return "c"; }
        }

        public string AssemblyId
        {
            get
            {
                return GeneratedIdentifier(Assembly.GetName().Name).Replace('.', '_');
            }
        }

        public override void Process()
        {
            GenerateFilePreamble();

            PushBlock();
            WriteLine("#include \"{0}.h\"", Unit.Name);
            WriteLine("#include <mono/jit/jit.h>");
            WriteLine("#include <mono/metadata/assembly.h>");
            WriteLine("#include <mono/metadata/object.h>");
            WriteLine("#include <mono/metadata/mono-config.h>");
            WriteLine("#include <mono/metadata/debug-helpers.h>");
            WriteLine("#include <stdlib.h>");
            WriteLine("#include \"mono_managed_to_native.h\"");

            PopBlock(NewLineKind.BeforeNextBlock);

            PushBlock();
            WriteLine("MonoDomain* {0};", GeneratedIdentifier("mono_domain"));
            WriteLine("bool {0};", GeneratedIdentifier("mono_initialized"));

            WriteLine("MonoAssembly* {0}_assembly;", AssemblyId);
            WriteLine("MonoImage* {0}_image;", AssemblyId);
            PopBlock(NewLineKind.BeforeNextBlock);

            GenerateMonoInitialization();
            GenerateAssemblyLoad();

            VisitDeclContext(Unit);
        }

        public override bool VisitEnumDecl(Enumeration @enum)
        {
            return true;
        }

        public override bool VisitClassDecl(Class @class)
        {
            PushBlock();
            WriteLine("static MonoClass* {0}_class = 0;", @class.QualifiedName);
            PopBlock(NewLineKind.BeforeNextBlock);

            PushBlock();
            WriteLine("struct {0}", @class.Name);
            WriteStartBraceIndent();

            WriteLine("MonoClass* _class;", @class.QualifiedName);
            WriteLine("uint32_t _handle;", @class.QualifiedName);

            PopIndent();
            WriteLine("};");
            PopBlock(NewLineKind.BeforeNextBlock);

            GenerateClassLookup(@class);

            VisitDeclContext(@class);

            return true;
        }

        public void GenerateMonoInitialization()
        {
            PushBlock();
            WriteLine("static void {0}()", GeneratedIdentifier("initialize_mono"));
            WriteStartBraceIndent();

            var initializedId = GeneratedIdentifier("mono_initialized");
            WriteLine("if ({0})", initializedId);
            WriteLineIndent("return;");

            WriteLine("mono_config_parse(NULL);");

            var domainName = "mono_managed_to_native_binding";
            var version = "v4.0.30319";
            WriteLine("{0} = mono_jit_init_version(\"{1}\", \"{2}\");",
                GeneratedIdentifier("mono_domain"), domainName, version);
            WriteLine("{0} = true;", initializedId);

            WriteCloseBraceIndent();
            PopBlock(NewLineKind.BeforeNextBlock);
        }

        public void GenerateAssemblyLoad()
        {
            var assemblyName = Assembly.GetName().Name;
            var assemblyLookupId = GeneratedIdentifier(string.Format("lookup_assembly_{0}",
                assemblyName.Replace('.', '_')));

            PushBlock();
            WriteLine("static void {0}()", assemblyLookupId);
            WriteStartBraceIndent();

            var monoAssemblyName = string.Format("{0}_assembly", AssemblyId);
            WriteLine("if ({0})", monoAssemblyName);
            WriteLineIndent("return;");

            WriteLine("const char* assembly = mono_managed_to_native_search_assembly(\"{0}.dll\");",
                assemblyName);

            WriteLine("{0} = mono_domain_assembly_open({1}, assembly);",
                monoAssemblyName, GeneratedIdentifier("mono_domain"));

            var monoImageName = string.Format("{0}_image", AssemblyId);
            WriteLine("{0} = mono_assembly_get_image({1});", monoImageName,
                monoAssemblyName);

            WriteCloseBraceIndent();
            PopBlock(NewLineKind.BeforeNextBlock);
        }

        public void GenerateClassLookup(Class @class)
        {
            PushBlock();

            var classLookupId = GeneratedIdentifier(string.Format("lookup_class_{0}",
                @class.QualifiedName.Replace('.', '_')));
            WriteLine("static void {0}()", classLookupId);
            WriteStartBraceIndent();

            var classId = string.Format("{0}_class", @class.QualifiedName);

            WriteLine("if ({0} == 0)", classId);
            WriteStartBraceIndent();

            WriteLine("{0}();", GeneratedIdentifier("initialize_mono"));

            var assemblyName = Assembly.GetName().Name;
            var assemblyLookupId = GeneratedIdentifier(string.Format("lookup_assembly_{0}",
                assemblyName.Replace('.', '_')));
            WriteLine("{0}();", assemblyLookupId);

            var @namespace = string.Empty;
            var ids = string.Join(", ",
                @class.QualifiedName.Split('.').Select(n => string.Format("\"{0}\"", n)));

            var monoImageName = string.Format("{0}_image", AssemblyId);
            WriteLine("{0} = mono_class_from_name({1}, \"{2}\", \"{3}\");",
                classId, monoImageName, @namespace, @class.OriginalName);
            WriteCloseBraceIndent();
            WriteCloseBraceIndent();

            PopBlock(NewLineKind.BeforeNextBlock);
        }

        public void GenerateMethodLookup(Method method)
        {
            var methodNameId = GeneratedIdentifier("method_name");
            WriteLine("const char {0}[] = \"{1}\";", methodNameId, method.OriginalName);
            var descId = GeneratedIdentifier("desc");

            WriteLine("MonoMethodDesc* {0} = mono_method_desc_new({1}, /*include_namespace=*/true);",
                descId, methodNameId);

            var methodId = GeneratedIdentifier("method");

            var @class = method.Namespace as Class;
            var classId = string.Format("{0}_class", @class.QualifiedName);

            WriteLine("MonoMethod* {0} = mono_method_desc_search_in_class({1}, {2});",
                methodId, descId, classId);

            WriteLine("mono_method_desc_free({0});", descId);

            var retType = method.ReturnType;
            var needsReturn = !retType.Type.IsPrimitiveType(PrimitiveType.Void);

            var @return = needsReturn ? " 0" : string.Empty;
            WriteLine("if ({0} == 0)", methodId);
                WriteLineIndent("return{0};", @return);
        }

        public void GenerateMethodGCHandleLookup(Method method)
        {
            var @class = method.Namespace as Class;
            var classId = string.Format("{0}_class", @class.QualifiedName);
            var instanceId = GeneratedIdentifier("instance");

            if (method.IsConstructor)
            {
                WriteLine("{0}* object = ({0}*) calloc(1, sizeof({0}));", @class.Name);
                WriteLine("MonoObject* {0} = mono_object_new({1}, {2});",
                    instanceId, GeneratedIdentifier("mono_domain"), classId);
                WriteLine("object->_handle = mono_gchandle_new({0}, /*pinned=*/false);",
                    instanceId);
            }
            else if (!method.IsStatic)
            {
                WriteLine("MonoObject* {0} = mono_gchandle_get_target(object->_handle);",
                    instanceId);
            }
        }

        public void GenerateMethodInvocation(Method method)
        {
            GenerateMethodGCHandleLookup(method);

            var paramsToMarshal = method.Parameters.Where(p => !p.IsImplicit);
            var numParamsToMarshal = paramsToMarshal.Count();

            var argsId = "0";

            if (numParamsToMarshal > 0)
            {
                argsId = GeneratedIdentifier("args");
                WriteLine("void* {0}[{1}];", argsId, numParamsToMarshal);
            }

            int paramIndex = 0;
            foreach (var param in paramsToMarshal)
            {
                var ctx = new MarshalContext(Driver)
                {
                    ArgName = param.Name,
                    Parameter = param
                };

                var marshal = new CMarshalNativeToManaged(ctx);
                param.QualifiedType.CMarshalToManaged(marshal);

                if (!string.IsNullOrWhiteSpace(marshal.Context.SupportBefore))
                    Write(marshal.Context.SupportBefore);

                WriteLine("{0}[{1}] = {2};", argsId, paramIndex++,
                    marshal.Context.Return.ToString());
            }

            var exceptionId = GeneratedIdentifier("exception");
            WriteLine("MonoObject* {0} = 0;", exceptionId);

            var resultId = GeneratedIdentifier("result");
            WriteLine("MonoObject* {0};", resultId);

            var methodId = GeneratedIdentifier("method");
            var instanceId = method.IsStatic ? "0" : GeneratedIdentifier("instance");

            WriteLine("{0} = mono_runtime_invoke({1}, {2}, {3}, &{4});", resultId,
                methodId, instanceId, argsId, exceptionId);
        }

        public override bool VisitMethodDecl(Method method)
        {
            PushBlock();

            GenerateMethodSignature(method);
            NewLine();
            WriteStartBraceIndent();

            var @class = method.Namespace as Class;
            var classLookupId = GeneratedIdentifier(string.Format("lookup_class_{0}",
                @class.QualifiedName.Replace('.', '_')));
            WriteLine("{0}();", classLookupId);

            GenerateMethodLookup(method);

            var retType = method.ReturnType;
            var needsReturn = !retType.Type.IsPrimitiveType(PrimitiveType.Void);

            GenerateMethodInvocation(method);

            var exceptionId = GeneratedIdentifier("exception");
            //WriteLine("if ({0})", exceptionId);
            //WriteLineIndent("return 0;");

            string returnCode = "0";

            // Marshal the method result to native code.
            if (!method.IsConstructor)
            {
                var resultId = GeneratedIdentifier("result");
                var ctx = new MarshalContext(Driver)
                {
                    ArgName = resultId,
                    ReturnVarName = resultId,
                    ReturnType = retType
                };

                var marshal = new CMarshalManagedToNative(ctx);
                retType.CMarshalToNative(marshal);

                if (!string.IsNullOrWhiteSpace(marshal.Context.SupportBefore))
                    Write(marshal.Context.SupportBefore);

                returnCode = marshal.Context.Return.ToString();
            }
            else
            {
                returnCode = "object";
            }

            if (method.IsConstructor || needsReturn)
                WriteLine("return {0};", returnCode);

            WriteCloseBraceIndent();
            PopBlock(NewLineKind.BeforeNextBlock);

            return true;
        }
    }
}
