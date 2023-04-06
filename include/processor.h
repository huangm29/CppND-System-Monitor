#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();
  void acttime(long acttime);
  long int acttime();
  void totaltime(long totaltime);
  long int totaltime();

 private:
  long acttime_;
  long totaltime_;
};

#endif