#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

  // DONE: Declare any necessary private members
 private:
  long prev_idle{};
  long prev_non_idle{};
  long prev_total{};
};

#endif