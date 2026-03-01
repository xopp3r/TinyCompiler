#include <iostream>
#include <fstream>
#include <print>
#include <string_view>
import driver;

auto main(const int argc, char const* const* const argv) -> int {

    bool emit_ast = false, use_stdio = false;
    std::string_view in_filename, out_filename{"out.ll"};


    if (argc == 1) {
        std::println("No arguments provided. Use -help for instructions");
        std::exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        std::string_view arg{argv[i]};

        if (arg == "-help") {
            std::println(
                "Usage: program [OPTIONS]\n"
                "options:\n"
                "  -i [FILENAME]  -  specify input file, required\n" 
                "  -o [FILENAME]  -  specify output file, default: 'out.ll'\n"
                "  -stdio         -  use stdin and stdout as files\n"
                "  -emit-ast      -  produce source code ast\n"
                "  -help          -  display this message\n"
            );
            std::exit(EXIT_SUCCESS);

        } else if (arg == "-o") {
            if (i + 1 < argc) out_filename = argv[++i];
            else {
                std::println("Error: missing filename after -o");
                std::exit(EXIT_FAILURE);
            } 
        } else if (arg == "-i") {
            if (i + 1 < argc) in_filename = argv[++i];
            else {
                std::println("Error: missing filename after -i");
                std::exit(EXIT_FAILURE);
            } 
        } else if (arg == "-stdio") {
            use_stdio = true;
        } else if (arg == "-emit-ast") {
            emit_ast = true;
        } else {
            std::println("Error: unknown argument '{}'", arg);
            std::exit(EXIT_FAILURE);
        }
    }


    if (not use_stdio and in_filename.empty()) {
        std::println("Error: input file required (use -i [FILENAME] or -stdio)");
        std::exit(EXIT_FAILURE);
    } else if (use_stdio and not in_filename.empty()) {
        std::println("Error: input file ignored because -stdio specified");
        std::exit(EXIT_FAILURE);
    }


    if (use_stdio) {

        tc::compile(std::cin, std::cout, out_filename, emit_ast);

    } else {
        std::ifstream in_file(in_filename.data()); 
        if (!in_file) {
            std::println("Error: cannot open input file: '{}'", in_filename);
            std::exit(EXIT_FAILURE);
        }

        std::ofstream out_file(out_filename.data());
        if (!out_file) {
            std::println("Error: cannot create output file: '{}'", out_filename);
            std::exit(EXIT_FAILURE);
        }

        tc::compile(in_file, out_file, out_filename, emit_ast);
    }
}
