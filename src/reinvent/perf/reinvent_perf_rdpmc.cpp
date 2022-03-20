#include <perf/reinvent_perf_rdpmc.h>

#include <sched.h>                                                                                                      
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

namespace Reinvent {

Perf::Rdpmc::Rdpmc(bool pin)
{                                                                                                                       
  // Pin caller's thread if requested
  if (pin) {
    pinCpu();
  }

  int core = cpu();

  wrmsr(0x38d, core, 0);
  wrmsr(0x186, core, 0);
  wrmsr(0x187, core, 0);
  wrmsr(0x188, core, 0);
  wrmsr(0x189, core, 0);
  wrmsr(0x309, core, 0);
  wrmsr(0x30a, core, 0);
  wrmsr(0x30b, core, 0);
  wrmsr(0xc1,  core, 0);
  wrmsr(0xc2,  core, 0);
  wrmsr(0xc3,  core, 0);
  wrmsr(0xc4,  core, 0);
  wrmsr(0x390, core, 0);
  wrmsr(0x38f, core, 0x70000000f);
  
  const unsigned long base(0x410000);
  wrmsr(0x186, core, base|Rdpmc::Event::UNHALTED_CORE_CYCLES);
  wrmsr(0x187, core, base|Rdpmc::Event::INSTRUCTIONS_RETIRED);
  wrmsr(0x188, core, base|Rdpmc::Event::LLC_REFERENCES);
  wrmsr(0x189, core, base|Rdpmc::Event::LLC_MISSES);
} 

int Perf::Rdpmc::cpu() const {
  return sched_getcpu();
}

int Perf::Rdpmc::pinCpu() {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu(), &mask);

  if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
      fprintf(stderr, "Error: Could not pin thread to core %d\n", cpu());
      return 1;
  }
  
  return 0;
}

int Perf::Rdpmc::rdmsr(unsigned int reg, int cpu, unsigned long *value) {
  unsigned long data;
  int fd;
  char msr_file_name[64];
  unsigned int bits;

  sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
  fd = open(msr_file_name, O_RDONLY);
  if (fd < 0) {
    if (errno == ENXIO) {
      fprintf(stderr, "rdmsr: No CPU %d\n", cpu);
      return 2;
    } else if (errno == EIO) {
      fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n", cpu);
      return 3;   
    } else {
      perror("rdmsr: open");
      return 4;
    }
  }

  if (pread(fd, &data, sizeof data, reg) != sizeof data) {
    if (errno == EIO) {
      fprintf(stderr, "rdmsr: CPU %d cannot read " "MSR 0x%x\n", cpu, reg);
      close(fd);
      return 5;
    } else {
      perror("rdmsr: pread");
      close(fd);
      return 6;
    }
  }

  close(fd);

  const unsigned int highbit = 63, lowbit = 0;                                                                                  
  bits = highbit - lowbit + 1;
  if (bits < 64) {
    /* Show only part of register */
    data >>= lowbit;
    data &= (1ULL << bits) - 1;
  }

  *value = data;
  return 0;
}

int Perf::Rdpmc::wrmsr(unsigned int reg, int cpu, unsigned long data) {
  int fd;
  char msr_file_name[64];

  printf("wrmsr reg 0x%04x cpu %02d data 0x%016lx\n", reg, cpu, data);

  sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
  fd = open(msr_file_name, O_WRONLY);
  if (fd < 0) {
    if (errno == ENXIO) {
      fprintf(stderr, "wrmsr: No CPU %d\n", cpu);
      return 2;
    } else if (errno == EIO) {
      fprintf(stderr, "wrmsr: CPU %d doesn't support MSRs\n", cpu);
      return 3;
    } else {
      perror("wrmsr: open");
      return 4;
    }
  }

  if (pwrite(fd, &data, sizeof data, reg) != sizeof data) {
    if (errno == EIO) {
      fprintf(stderr, "wrmsr: CPU %d cannot set MSR 0x%x to 0x%lx\n", cpu, reg, data);
      close(fd);
      return 5;
    } else {
      perror("wrmsr: pwrite");
      close(fd);
      return 6;
    }
  }

  close(fd);
  return 0;
}

}; // namespace Reinvent
