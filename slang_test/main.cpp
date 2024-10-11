#include <iostream>
#include <filesystem>
#include "slang-com-ptr.h"
#include "slang.h"

static const char* NULL_CHAR = "null";

// Many Slang API functions return detailed diagnostic information
// (error messages, warnings, etc.) as a "blob" of data, or return
// a null blob pointer instead if there were no issues.
//
// For convenience, we define a subroutine that will dump the information
// in a diagnostic blob if one is produced, and skip it otherwise.
//
void diagnose_if_needed(slang::IBlob* diagnosticsBlob) {
    if( diagnosticsBlob != nullptr ) {
        printf("Diagnostics message:\n%s", static_cast<const char *>(diagnosticsBlob->getBufferPointer()));
    }
}

const char* safe_c_str(const char* input) {
    if (nullptr == input) {
        return NULL_CHAR;
    }
    return input;
}

void print_layout(slang::ProgramLayout* program_layout) {
    if ( program_layout != nullptr ) {
        std::cout << "Num entrypoint: " << program_layout->getEntryPointCount() << '\t';
        std::cout << "Num parameter count: " << program_layout->getParameterCount() << '\t';
        std::cout << "Num hashed string: " << program_layout->getHashedStringCount() << '\t';
        std::cout << "Num type parameter: " << program_layout->getTypeParameterCount() << '\t';
        std::cout << '\n';
        std::cout << "Global constant buffer size: " << program_layout->getGlobalConstantBufferSize() << '\t';
        std::cout << "Global constant buffer binding: " << program_layout->getGlobalConstantBufferBinding() << '\t';
        std::cout << std::endl;
        for (int i = 0; i < program_layout->getParameterCount(); ++i) {
            auto* parameter_info = program_layout->getParameterByIndex(i);
            std::cout << "[slot=" << parameter_info->getBindingIndex() << ", space=" << parameter_info->getBindingSpace() << ", category=" << parameter_info->getCategory() << "] " << parameter_info->getName() << std::endl;
        }
        for (int i = 0; i < program_layout->getTypeParameterCount(); ++i) {
            auto* parameter_info = program_layout->getTypeParameterByIndex(i);
            std::cout << "[Type Parameter] " << safe_c_str(parameter_info->getName()) << " : " << parameter_info->getIndex() << std::endl;
        }
        std::cout << std::endl;
    }
}

void print_variable_info(slang::VariableReflection* variable_reflection) {
    std::cout << variable_reflection->getName() << ": ";
    std::cout << variable_reflection->getType()->getName();
    std::cout << std::endl;
}

void print_function_info(slang::FunctionReflection* function_reflection) {
    const char* name = function_reflection->getName();
    std::cout << "=== Begin " << name << " (function) ===\n";
    std::cout << "Num parameters: " << function_reflection->getParameterCount() << '\t';
    std::cout << "Num user attribute: " << function_reflection->getUserAttributeCount() << '\n';
    std::cout << "Parameters: \n";
    for (int i = 0; i < function_reflection->getParameterCount(); ++i) {
        slang::VariableReflection* variable_reflection = function_reflection->getParameterByIndex(i);
        std::cout << "\t";
        print_variable_info(variable_reflection);
    }
    std::cout << "=== End " << name << " ===\n" << std::endl;
}

int main() {
    const std::filesystem::path current_working_dir = std::filesystem::current_path();
    const std::string cwd_str = current_working_dir.generic_string();
    std::cout << "CWD: " << current_working_dir << std::endl;

    Slang::ComPtr<slang::IGlobalSession> slang_global_session;
    createGlobalSession(slang_global_session.writeRef());

    slang::TargetDesc target_desc{};
    target_desc.format = SLANG_SPIRV;
    target_desc.profile = slang_global_session->findProfile("spirv_1_6");
    target_desc.flags = SLANG_TARGET_FLAG_GENERATE_SPIRV_DIRECTLY | SLANG_TARGET_FLAG_GENERATE_WHOLE_PROGRAM;
    target_desc.forceGLSLScalarBufferLayout = true;

    slang::SessionDesc session_desc{};
    session_desc.targets = &target_desc;
    session_desc.targetCount = 1;
    std::vector<const char*> search_path;
    search_path.push_back(cwd_str.c_str());
    session_desc.searchPaths = search_path.data();
    session_desc.searchPathCount = 1;
    session_desc.preprocessorMacroCount = 0;

    Slang::ComPtr<slang::ISession> session;
    if (SLANG_FAILED(slang_global_session->createSession(session_desc, session.writeRef()))) {
        return 1;
    }

    Slang::ComPtr<slang::IBlob> diagnostic_blob;
    slang::IModule* module = session->loadModule("test", diagnostic_blob.writeRef());

    diagnose_if_needed(diagnostic_blob);

    std::vector<slang::IComponentType*> program_components{};
    program_components.push_back(module);

    for (int i = 0; i < module->getDefinedEntryPointCount(); ++i) {
        Slang::ComPtr<slang::IEntryPoint> entry_point;
        if (SLANG_FAILED(module->getDefinedEntryPoint(i, entry_point.writeRef()))) {
            std::cerr << "No way!" << std::endl;
            continue;
        }
        print_function_info(entry_point->getFunctionReflection());
        program_components.push_back(entry_point);
    }

    Slang::ComPtr<slang::IComponentType> linked_program;
    if (SLANG_FAILED(session->createCompositeComponentType(program_components.data(), program_components.size(), linked_program.writeRef(), diagnostic_blob.writeRef()))) {
        std::cerr << "No wayyy!" << std::endl;
        return 1;
    }
    diagnose_if_needed(diagnostic_blob);

    print_layout(linked_program->getLayout());

    return 0;
}
