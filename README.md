CrISA-X: Unleashing Performance Excellence in
Lightweight Symmetric Cryptography for
Extendable and Deeply Embedded Processors


The efficient execution of a Lightweight Cryptography (LWC) algorithm is essential for
edge computing platforms. Dedicated Instruction Set Extensions (ISEs) are often included
for this purpose. We propose the CrISA-X-a Cryptography Instruction Set Architecture
eXtensions designed to improve cryptographic latency on extendable processors.CrISA-X,
provides enhanced speed of various algorithms simultaneously while optimizing ISA adaptability, a feat yet to be accomplished. The extension, diverse for several computation
levels, is first tailored explicitly for individual algorithms and sets of LWC algorithms,
depending on performance, frequency, and area trade-offs. By diligently applying the
Min-Max optimization technique, we have configured these extensions to achieve a delicate
balance between performance, area utilization, code size, etc. Our study presents empirical
evidence of the performance enhancement achieved on a synthesis modular RISC processor.
We offer a framework for creating optimized processor hardware and ISA extensions. The
CrISA-X outperforms ISA extensions by delivering significant performance boosts between
3x to 17x while experiencing a relative area cost increase of +12% and +47% in LUTs.
Notably, as one important example, the utilization of the ASCON algorithm yields a 10x
performance boost in contrast to the base ISA instruction implementation
