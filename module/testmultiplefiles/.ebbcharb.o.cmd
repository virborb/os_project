cmd_/home/simon/Documents/operativsys/os_project/module/testmultiplefiles/ebbcharb.o :=  gcc-5 -Wp,-MD,/home/simon/Documents/operativsys/os_project/module/testmultiplefiles/.ebbcharb.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/5/include -I/usr/src/linux-headers-4.8.0-1-common/arch/x86/include -I./arch/x86/include/generated/uapi -I./arch/x86/include/generated  -I/usr/src/linux-headers-4.8.0-1-common/include -I./include -I/usr/src/linux-headers-4.8.0-1-common/arch/x86/include/uapi -I/usr/src/linux-headers-4.8.0-1-common/include/uapi -I./include/generated/uapi -include /usr/src/linux-headers-4.8.0-1-common/include/linux/kconfig.h -I/usr/src/linux-headers-4.8.0-1-common//home/simon/Documents/operativsys/os_project/module/testmultiplefiles -I/home/simon/Documents/operativsys/os_project/module/testmultiplefiles -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mtune=generic -mno-red-zone -mcmodel=kernel -funit-at-a-time -maccumulate-outgoing-args -DCONFIG_X86_X32_ABI -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -DCONFIG_AS_FXSAVEQ=1 -DCONFIG_AS_SSSE3=1 -DCONFIG_AS_CRC32=1 -DCONFIG_AS_AVX=1 -DCONFIG_AS_AVX2=1 -DCONFIG_AS_SHA1_NI=1 -DCONFIG_AS_SHA256_NI=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -Wno-maybe-uninitialized -O2 --param=allow-store-data-races=0 -Wframe-larger-than=2048 -fstack-protector-strong -Wno-unused-but-set-variable -fno-var-tracking-assignments -g -pg -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time -Werror=incompatible-pointer-types -DCC_HAVE_ASM_GOTO  -DMODULE  -DKBUILD_BASENAME='"ebbcharb"'  -DKBUILD_MODNAME='"ebbchar"' -c -o /home/simon/Documents/operativsys/os_project/module/testmultiplefiles/.tmp_ebbcharb.o /home/simon/Documents/operativsys/os_project/module/testmultiplefiles/ebbcharb.c

source_/home/simon/Documents/operativsys/os_project/module/testmultiplefiles/ebbcharb.o := /home/simon/Documents/operativsys/os_project/module/testmultiplefiles/ebbcharb.c

deps_/home/simon/Documents/operativsys/os_project/module/testmultiplefiles/ebbcharb.o := \
  /home/simon/Documents/operativsys/os_project/module/testmultiplefiles/ebbcharb.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/kernel.h \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/panic/timeout.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /usr/lib/gcc/x86_64-linux-gnu/5/include/stdarg.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/linkage.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/kasan.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/gcov/kernel.h) \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/linux/types.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/uapi/asm/types.h \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/asm-generic/types.h \
  /usr/src/linux-headers-4.8.0-1-common/include/asm-generic/int-ll64.h \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/asm-generic/int-ll64.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/uapi/asm/bitsperlong.h \
  /usr/src/linux-headers-4.8.0-1-common/include/asm-generic/bitsperlong.h \
    $(wildcard include/config/64bit.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/asm-generic/bitsperlong.h \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/linux/posix_types.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/stddef.h \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/linux/stddef.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/x86/32.h) \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/uapi/asm/posix_types_64.h \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/asm-generic/posix_types.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/stringify.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/trim/unused/ksyms.h) \
    $(wildcard include/config/unused/symbols.h) \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/64.h) \
    $(wildcard include/config/x86/alignment/16.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/types.h \
    $(wildcard include/config/have/uid16.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/bitops.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/bitops.h \
    $(wildcard include/config/x86/cmov.h) \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/alternative.h \
    $(wildcard include/config/smp.h) \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/asm.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/rmwcc.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/barrier.h \
    $(wildcard include/config/x86/ppro/fence.h) \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/nops.h \
    $(wildcard include/config/mk7.h) \
    $(wildcard include/config/x86/p6/nop.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/asm-generic/barrier.h \
  /usr/src/linux-headers-4.8.0-1-common/include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/asm-generic/bitops/sched.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/arch_hweight.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/cpufeatures.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/required-features.h \
    $(wildcard include/config/x86/minimum/cpu/family.h) \
    $(wildcard include/config/math/emulation.h) \
    $(wildcard include/config/x86/pae.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
    $(wildcard include/config/x86/use/3dnow.h) \
    $(wildcard include/config/matom.h) \
    $(wildcard include/config/paravirt.h) \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/x86/intel/mpx.h) \
    $(wildcard include/config/x86/intel/memory/protection/keys.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/asm-generic/bitops/const_hweight.h \
  /usr/src/linux-headers-4.8.0-1-common/include/asm-generic/bitops/le.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/uapi/asm/byteorder.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/byteorder/little_endian.h \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/linux/byteorder/little_endian.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/swab.h \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/linux/swab.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/uapi/asm/swab.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/byteorder/generic.h \
  /usr/src/linux-headers-4.8.0-1-common/include/asm-generic/bitops/ext2-atomic-setbit.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/typecheck.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/printk.h \
    $(wildcard include/config/message/loglevel/default.h) \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk/nmi.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
    $(wildcard include/config/debug/rodata.h) \
    $(wildcard include/config/lto.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/kern_levels.h \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/linux/kernel.h \
  /usr/src/linux-headers-4.8.0-1-common/include/uapi/linux/sysinfo.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/cache.h \
    $(wildcard include/config/x86/l1/cache/shift.h) \
    $(wildcard include/config/x86/internode/cache/shift.h) \
    $(wildcard include/config/x86/vsmp.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/dynamic_debug.h \
    $(wildcard include/config/jump/label.h) \
  /usr/src/linux-headers-4.8.0-1-common/include/linux/jump_label.h \
  /usr/src/linux-headers-4.8.0-1-common/arch/x86/include/asm/jump_label.h \

/home/simon/Documents/operativsys/os_project/module/testmultiplefiles/ebbcharb.o: $(deps_/home/simon/Documents/operativsys/os_project/module/testmultiplefiles/ebbcharb.o)

$(deps_/home/simon/Documents/operativsys/os_project/module/testmultiplefiles/ebbcharb.o):
