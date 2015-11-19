cmd_arch/arm/lib/muldi3.o := /home/w4118/hmwk5-prog/flo-kernel/scripts/gcc-wrapper.py arm-eabi-gcc -Wp,-MD,arch/arm/lib/.muldi3.o.d  -nostdinc -isystem /home/w4118/utils/arm-eabi-4.6/bin/../lib/gcc/arm-eabi/4.6.x-google/include -I/home/w4118/hmwk5-prog/flo-kernel/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/w4118/hmwk5-prog/flo-kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-goldfish/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float        -c -o arch/arm/lib/muldi3.o arch/arm/lib/muldi3.S

source_arch/arm/lib/muldi3.o := arch/arm/lib/muldi3.S

deps_arch/arm/lib/muldi3.o := \
  /home/w4118/hmwk5-prog/flo-kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/w4118/hmwk5-prog/flo-kernel/arch/arm/include/asm/linkage.h \

arch/arm/lib/muldi3.o: $(deps_arch/arm/lib/muldi3.o)

$(deps_arch/arm/lib/muldi3.o):
