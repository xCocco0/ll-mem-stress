# Linked list memory stressor
This repository contains the source files for a memory stressor based on linked list transversal. The operations implemented are divided into:
- First part - allocate memory for the stressor that will be used for the linked list transversal
- Seconds part - jump into an infinite loop of only load operations (loop is unrolled by a factor 32 to reduce loop overheads)

## How to compile
To compile a few ready-made examples

      make

Instead, to compile a new executable

      make BITS=<comma separated list of numbers>

More info on parameters can be found below

## Parameters that can be set in the source

- ``BITS``\
  This is the most important parameter, it specifies the pattern of the accesses that the list will perform. It should be formatted as a comma separated list of integers, representing the bits that compose the variable part of the address.\
  For instance, ``BITS=0,1`` will perform accesses to addresses ``0x0->0x1->0x2->0x3-->0x0...`` (relative to the base address).\
  Watch out, because the ordering of the list counts: ``BITS=1,0`` will results in the access pattern ``0x0->0x2->0x3->0x1`` (in binary ``0b00->0b10->0b11->0b01``).\ In a nutshell, the first bits in the list are the ones that change more frequently, whereas the latter more slowly. This is a very nice feature to design cache stressors, as it allows set-wise or way-wise accesses.
- ``BASE``\
  An additive value to be added to the base address retrieved by malloc
- ``DEPTH``\
  The size of data to be read per list node (cannot be any less than the size of a pointer)
- ``MEM_SIZE``\
  The size of the memory region to be populated with the list nodes. The default value uses a 1MB region from a 1MB huge page (having a MEM_SIZE larger than a page will not guarantee that memory pages will be sequentially allocated)
