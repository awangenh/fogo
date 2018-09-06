#include <gnm/controller.hpp>

int main(int argn, char** argv) {

  //if (argn != 8) return 1;

  //teste2 meuteste(argv[1],argv[2],atof(argv[3]),atof(argv[4]),atoi(argv[5]),atoi(argv[6]),atof(argv[7]));
  gnm::controller::controller gnm2;
  //gnm2.runGNM(argv[1],argv[2],atof(argv[3]),atof(argv[4]),atoi(argv[5]),atoi(argv[6]),atof(argv[7]));

  gnm2.runGNM_console(argn,argv);

  //gnm2.run();

  return 0;
}
