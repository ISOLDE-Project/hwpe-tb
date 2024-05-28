
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "CLI11.hpp"
#include "elfLoader.hpp"

int main(int argc, char **argv) {
  std::string binaryFile; // assign to default
  std::string outputFile;
  std::string traceFile;

  CLI::App app{"ELF loader"};
  app.add_option("-f,--file", binaryFile,
                 "Specifies the RISC-V program binary file (elf)")
      ->required();
  app.add_option(
      "-o,--output", outputFile,
      "Specifies the output file (standard output of the running program)");
  app.add_option("-t,--trace-file", traceFile,
                 "Specifies trace file for simulator output");
  CLI11_PARSE(app, argc, argv);

  //
  ELFLoader loader;
  loader.readElf(binaryFile);
  fprintf(stderr, "PROGRAM_SIZE=%u Bytes, %.2f kB \n", loader.max_idx(),
          (float)(loader.max_idx() / 1024.));
  //
  std::string name("code_image");
  FILE *pyFile = loader.openOrDefault(std::string("code_image.py"), "wb", NULL);
  fprintf(pyFile, "import numpy as np\n\n");

  py_pretty_print<uint32_t>(pyFile, name, loader);
  fprintf(pyFile, "\n\nstart=%u\n\n", loader.getStartAddress());
  //
  FILE *csv_File =
      loader.openOrDefault(std::string("code_image.csv"), "wb", NULL);
  csv_pretty_print<uint32_t>(csv_File, name, loader);

  return 0;
}
