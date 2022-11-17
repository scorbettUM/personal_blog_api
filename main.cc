#include "cli/Cli.h"


int main(int argc, char** argv) {


    auto command_line = cli::Cli();
    return command_line.execute(argc, argv);


}
