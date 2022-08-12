<div id="doc" class="markdown-body container-fluid comment-inner comment-enabled" data-hard-breaks="true" style="">

<div class="alert alert-success">

# [<span class="octicon octicon-link"></span>](#OT-Lab-1-Assignment-Assembly-Patterns "OT-Lab-1-Assignment-Assembly-Patterns")<span>OT Lab 1 Assignment: Assembly Patterns</span>

**<span>Name: Naghmeh Mohammadifar</span>**

</div>

## [<span class="octicon octicon-link"></span>](#Part-1-Understanding-Assembly "Part-1-Understanding-Assembly")<span>Part 1: Understanding Assembly</span>

### [<span class="octicon octicon-link"></span>](#Task-1-Preparation "Task-1-Preparation")<span>Task 1: Preparation</span>

<div class="alert alert-info">

1.  <span>Choose any debugger/disassembler what supports the given task
    architecture (32bit & 64bit). Some of options:</span>
      - <span>IDA</span>
      - <span>Ghidra</span>
      - <span>Hopper</span>
      - <span>Radare2</span>
      - <span>gdb</span>
      - <span>OllyDbg (for Windows OS)</span>
      - <span>Immunity Debugger (for Windows OS)</span>
2.  <span>You have to check and analyze what does a binary do before
    running it. You have no idea what file we are working with, even if
    it is initially considered as legitimate application.</span>
3.  <span>Prepare a Linux VM for this lab, as you might need to disable
    ASLR.</span>

</div>

#### [<span class="octicon octicon-link"></span>](#Implementation "Implementation")<span>Implementation:</span>

### [<span class="octicon octicon-link"></span>](#Task-2-Theory "Task-2-Theory")<span>Task 2: Theory</span>

<div class="alert alert-info">

1.  <span>What is the difference between mov and lea commands?</span>
2.  <span>What is ASLR , and why do we need it?</span>
3.  **<span>How can the debugger insert a breakpoint in the debugged
    binary/application?</span>**

</div>

#### [<span class="octicon octicon-link"></span>](#Implementation1 "Implementation1")<span>Implementation:</span>

1.  <span>Lea or Load Effective Address is when we want to load and have
    the address of the item for later usecases but with mov we load the
    item itself (the actual value)</span>
2.  <span>When program loads into memory, every part will be in an
    address and if the attacker exactly knows where the stack is, at
    which address does the function start, etc. he can create the buffer
    overflow attack. As a protection, Address Space Layout Randomization
    (ASLR) is created and in each run of the program, will change the
    addresses and randomize the locations of different parts of the
    program in memory (stack, heap, …)</span>
3.  <span>Debugger adds a breakpoint instruction that is an instruction
    inserting a fault or similar task that halts the application.</span>

### [<span class="octicon octicon-link"></span>](#Task-3-Disassembly "Task-3-Disassembly")<span>Task 3: Disassembly</span>

<div class="alert alert-info">

1.  <span>Disable ASLR in your Linux VM using the following
    command:</span>

<!-- end list -->

    sudo sysctl -w kernel.randomize_va_space=0

<span>Or</span>

    echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

</div>

<span>Actually before I start, I want to leave this photo for myself
here:</span>

![](https://i.imgur.com/08xSdWk.png)  
<span>Register refrences</span>

<div class="alert alert-info">

2.  <span>Load the binaries ( sample 32 and sample 64 from part1.zip)
    into a disassembler/debugger.</span>
3.  <span>Does the function prologue and epilogue differ in 32bit and
    64bit? What about calling conventions?</span>

</div>

<span>I used r2 (radare2) and this is the assembly code of main function
of the both files.</span>

![](https://i.imgur.com/xgAlo4N.png)

<span>Figure 1: Sample64 assembly</span>

![](https://i.imgur.com/Ihf3OGG.png)  
<span>Figure 2: Sample32 assembly</span>

<span>As you can see, yes they have differencies.</span>

<span>in sample32 we have these lines inaddition to the other ones which
are same between these two:</span>

    lea ecx, [argv]
    and esp, 0xfffffff0
    push dword [ecx - 4]

<span>They are done for stack alignment, which is required because of
SSE instructions and for some optimizations like padding in C for
variables. In this way, GCC will reserve 16-bytes stack (and stack
pointer with 0xfffffff0)</span>

<span>Also this part is different too:</span>

    push ebx
    push ecx

<span>It is a gcc implementation detail for the x86 code generator which
contain content from time of call.</span>  
<span>And then in the epilog they will pop from the stack and stack
alignment is done (lea esp,\[ecx-4\])</span>

<span>This part was also different in this two:</span>

    call sym.__x86.get_pc_thunk.ax

<span>Thats for generating “position-independent” executables which
means loading the program’s machine code at any address in (virtual)
memory and it’ll still be able to run. As I read, we need set up a
“global pointer” to completely use this feature of
independancy.</span>

<span>I am not 100% sure but I think the calling conventions are same in
x64 and x86.</span>  
<span>on the other side, I found this cheat sheet:</span>

![](https://i.imgur.com/niewnjs.png)  
<span>Figure: calling conventions cheat sheet</span>

<div class="alert alert-info">

4.  <span>Does function calls differ in 32bit and 64bit? What about
    argument passing?</span>
5.  <span>What does the command ldd do? “ ldd BINARY-NAME ”.</span>

</div>

4.  <span>No I think they are the same. But the registers are different
    (r represent x64 and e for x86)</span>

5.  <span>ldd command prints shared object (shared libraries) required
    by each program .</span>

![](https://i.imgur.com/2SDY7mF.png)

## [<span class="octicon octicon-link"></span>](#Part-2-Reverse-engineering "Part-2-Reverse-engineering")<span>Part 2: Reverse engineering</span>

### [<span class="octicon octicon-link"></span>](#Task-1-Theory "Task-1-Theory")<span>Task 1: Theory</span>

<div class="alert alert-info">

1.  <span>What kind of file did you receive (which arch? 32bit or
    64bit?)?</span>
2.  <span>What do stripped binaries mean?</span>
3.  <span>What are GOT and PLT ? Describe on the practical
    example.</span>
4.  <span>What are binary symbols in reverse engineering? How does it
    help?</span>

</div>

#### [<span class="octicon octicon-link"></span>](#Implementation2 "Implementation2")<span>Implementation:</span>

<span>I used the “file” command and there were these files:</span>

    bin1-5: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 3.2.0, BuildID[sha1]=ba3f21bfd29e03a056a158da7cf3ef2e7c113947, not stripped
    
    bin6: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, for GNU/Linux 3.2.0, BuildID[sha1]=be24706e7be9ba3fae96939e094dba08023c2461, stripped

<span>Main parts:</span>

  - <span>it is 64 bit executable</span>
  - <span>x86-64: 64-bit version of the x86 instruction set</span>
  - <span>A static library is linked into the final executable and it is
    a part of the executable. A dynamic library is loaded every time the
    executable is executed and remains separate from the executable as a
    DLL file.</span>

<!-- end list -->

2.  <span>Non-stripped binaries have debugging information built into
    it.Compiling an executable with gcc’s -g flag makes it with the
    debugging information. Whereas Strip binaries generally remove this
    debugging information from the exe which is not necessary for
    execution so as to reduce the size of the exe.</span>

3.  <span>The PLT (Procedure Linkage Table) and GOT (Global Offset
    Table) work together to perform the linking. ELF splits GOT in two
    tables:</span>

<!-- end list -->

  - <span>.got: storing the refrences for global variables</span>
  - <span>.got.plt: refrences for functions</span>

<span>PLT used to call external procedures/functions whose address isn’t
known in the time of linking, and is left to be resolved by the dynamic
linker at run time.</span>

![](https://i.imgur.com/maJ09Ay.png)

![](https://i.imgur.com/Ny82loP.png)  
<span>Figure: .got and .got.plt</span>

![](https://i.imgur.com/1gKSa24.png)  
<span>Figure: .plt</span>

4.  <span>Symbol tables are created during compilation and they are
    useful for debugging so in stripped mode they will be deleted after
    compilation finished. Dynamic symbols should remain in
    anycase.</span>

![](https://i.imgur.com/RzvOvh9.png)  
<span>Figure : symbol tables</span>

<span>As you can see, this tables holds the character representations of
the symbol names. If the value is nonzero, the value represents a string
table index that gives the symbol name.</span>

### [<span class="octicon octicon-link"></span>](#Task-2-Reverse-amp-Cracking "Task-2-Reverse-amp-Cracking")<span>Task 2: Reverse & Cracking</span>

<div class="alert alert-info">

<span>Inside the ZIP file (part2.zip), you will have multiple binaries (
bin 1-6 ), try to reverse them by recreating them using any programming
language of your choice (C is more preferred). You are given two simple
PE files as well:</span>

  - <span>task1.exe (it was found that this binary is dynamically linked
    to the debug version of Microsoft Visual C++ redistributable 2015.
    You can put this package together with Visual Studio installation,
    or add dll file that is inside the zip archive)</span>
  - <span>task2.exe Your task is to crack the program and find the
    correct password.</span>

</div>

#### [<span class="octicon octicon-link"></span>](#Implementation3 "Implementation3")<span>Implementation:</span>

**<span>Reverse Bin1:</span>**

<span>First lets look through the assembly:</span>

![](https://i.imgur.com/CpI7HLb.png)  
<span>Figure: bin1 assembly</span>

<span>This file has 4 major parts.</span>  
<span>In the first one it will call and use the time function</span>  
<span>With the edi = 0 which indicate the input for this
function.</span>  
<span>Then in the second one, it will call the localtime function with
the output of the pervious call (which is stored in rax) as the
input.</span>  
<span>The third one put the values in right registers for calling the
printf in 4’th section.</span>  
<span>The forth one will call printf with a format string.</span>

<span>I used this information and wrote the the following c file and
when I compiled it and looked through the assembly of it, mine and the
original one were identical.</span>

![](https://i.imgur.com/KtuUuml.png)  
<span>Figure : C code for reverse bin1</span>

![](https://i.imgur.com/fZx4ULn.png)  
<span>Figure : Assembly for reverse bin1</span>

<span>Actually this was how addresses assigned:</span>

    tm_isdst = rax+32
    tm_yday  = rax+28
    tm_wday  = rax+24
    tm_year  = rax+20
    tm_month = rax+16
    tm_day   = rax+12
    tm_hour  = rax+8
    tm_min   = rax+4
    tm_sec   = rax

<span>run the file:</span>

    gcc bin1_rev.c -o bin1_rev

<span>And same output:</span>

    0122-02-31 04:37:89

**<span>Reverse Bin2:</span>**

![](https://i.imgur.com/UWbO282.png)  
<span>Figure : bin2 assembly</span>

<span>As you can see, this code is consiste of 2 loops.</span>  
<span>Some information interesting for me in this assembly:</span>

  - <span>\[local\_64h\] is equal to i (our loop counter) and first it
    is assigned to 0 (mov qword\[local\_64\], 0)</span>
  - <span>\[rax+rax\] is for i\*2</span>
  - <span>\[rbp + rax\*4 -0x60\] is for address of the array we are
    holding the values. That 0x60 is the array total size which is equal
    to array size 21</span>

<span>I used this information and wrote the following c code for
it:</span>

![](https://i.imgur.com/p28cev3.png)  
<span>Figure : C code for reverse bin2</span>

![](https://i.imgur.com/OGHDfeo.png)  
<span>Figure : assembly for reverse bin2</span>

![](https://i.imgur.com/PPd8ntO.png)  
<span>Figure : The output of bin2 and bin2\_reverse</span>

**<span>Reverse Bin3:</span>**

<span>I believe this is same as bin2.</span>  
<span>First lets look through the assembly of this file and we can see
that they are the same.</span>

![](https://i.imgur.com/W0WJbDE.png)  
<span>Figure : bin3 assembly</span>

<span>Then I will get the hash of these two file and compare
them:</span>

    bin2:
    
    md5 9a6b3437acaaf6b905dc0a63cbc5a0fe
    sha1 6cfa97eec60ebc883dcac513c40a90741036b588
    sha256 97e4cc4efa44e95030ccb86c4be077b34ace2aac249ff9292e7426bfcf0fc5c1
    
    
    bin3:
    
    md5 9a6b3437acaaf6b905dc0a63cbc5a0fe
    sha1 6cfa97eec60ebc883dcac513c40a90741036b588
    sha256 97e4cc4efa44e95030ccb86c4be077b34ace2aac249ff9292e7426bfcf0fc5c1

<span>As you can see, there are totally same else the hash value
wouldn’t be identical.</span>

<span>Also I checked using diff command:</span>

![](https://i.imgur.com/IRihPYM.png)  
<span>Figure: diff command checking</span>

**<span>Reverse Bin4:</span>**

![](https://i.imgur.com/urRFmRs.png)  
<span>Figure : bin4 assembly</span>

<span>Actually this one was really easy and examined was the input value
even or odd . The only interesting parts there was:</span>

    and eax, 1
    test eax, eax
    jne 0x77c

<span>When we “and” a number with 1, if the num is even the result would
be 0 (0100 & 0001 = 0000) else the result would be 1 (0101 & 0001 =
0001)</span>  
<span>Then it would test. this operation in assembly is for updating the
flags (zf , …).</span>  
<span>jne says if the flag zf =0 (the result of and operation was 1 or
the number was even) we should jump to that address that will print “%d
is odd”.</span>

<span>I used this information and write the following code and after I
ran it, I had the same output. Also the assembly was the same as the
original one.</span>

![](https://i.imgur.com/Ie2ipnc.png)  
<span>Figure : C code for reverse bin1</span>

![](https://i.imgur.com/NWV3pXy.png)  
<span>Figure : assembly for reverse bin2</span>

![](https://i.imgur.com/6GazI2D.png)  
<span>Figure : graph base reverse bin2</span>

![](https://i.imgur.com/8ZIQzCV.png)  
<span>Figure : The output of bin2 and bin2\_reverse</span>

**<span>Reverse task1.exe:</span>**

<span>First I used the strings command to see what it will print for
us:</span>

![](https://i.imgur.com/6iDII63.png)  
<span>Figure : strings found in the file</span>

<span>Then I opened the file and search for the string “Username” to
take me to that part:</span>

![](https://i.imgur.com/uWbWFdS.png)  
<span>Figure : username and password</span>

<span>Actually I tried this value and it was the username and password
and then I tried to find where and how this values are used cause they
are pushed in the stack. Unfortunatly I wasnt successful and couldn’t
find the root. Would you please let me know about the logic? Thank you
in advance.</span>

**<span>Reverse task2.exe</span>**

![](https://i.imgur.com/lS3xFXO.png)

<span>as you can see it is comparing the input with a value</span>  
<span>I tried to create the value.</span>

<span>it was like this:</span>

<span>x = ds:dword\_403188 - ds:dword\_40318C + 19F97Fh</span>  
<span>x = 0F15EAh - 498F0h + 19F97Fh</span>

<span>Then I changed them to decimal :</span>

<span>x = 988650 - 301296 + 1702271 = 2389625</span>

<span>A</span>

## [<span class="octicon octicon-link"></span>](#Bonus "Bonus")<span>Bonus:</span>

<div class="alert alert-info">

<span>Describe the difference between PE and ELF executable files. Dhow
it by a practical example.</span>

</div>

-----

<span>References:</span>

1.  [<span>What is the difference between MOV and
    LEA?</span>](https://stackoverflow.com/questions/1699748/what-is-the-difference-between-mov-and-lea)
2.  [<span>What Is ASLR, and How Does It Keep Your Computer
    Secure?</span>](https://www.howtogeek.com/278056/what-is-aslr-and-how-does-it-keep-your-computer-secure/)
3.  

</div>

<div class="ui-toc dropup unselectable hidden-print" style="display:none;">

<div class="pull-right dropdown">

[**](# "Table of content")

<div class="toc">

  -   - [Part 1: Understanding
        Assembly](#Part-1-Understanding-Assembly "Part 1: Understanding Assembly")
          - [Task 1:
            Preparation](#Task-1-Preparation "Task 1: Preparation")
          - [Task 2: Theory](#Task-2-Theory "Task 2: Theory")
          - [Task 3:
            Disassembly](#Task-3-Disassembly "Task 3: Disassembly")
      - [Part 2: Reverse
        engineering](#Part-2-Reverse-engineering "Part 2: Reverse engineering")
          - [Task 1: Theory](#Task-1-Theory "Task 1: Theory")
          - [Task 2: Reverse &
            Cracking](#Task-2-Reverse-amp-Cracking "Task 2: Reverse & Cracking")
      - [Bonus:](#Bonus "Bonus:")

</div>

<div class="toc-menu">

[Expand all](#)[Back to top](#)[Go to bottom](#)

</div>

</div>

</div>

<div id="ui-toc-affix" class="ui-affix-toc ui-toc-dropdown unselectable hidden-print" data-spy="affix" style="top:17px;display:none;" data-null="" data-null="">

<div class="toc">

  -   - [Part 1: Understanding
        Assembly](#Part-1-Understanding-Assembly "Part 1: Understanding Assembly")
          - [Task 1:
            Preparation](#Task-1-Preparation "Task 1: Preparation")
          - [Task 2: Theory](#Task-2-Theory "Task 2: Theory")
          - [Task 3:
            Disassembly](#Task-3-Disassembly "Task 3: Disassembly")
      - [Part 2: Reverse
        engineering](#Part-2-Reverse-engineering "Part 2: Reverse engineering")
          - [Task 1: Theory](#Task-1-Theory "Task 1: Theory")
          - [Task 2: Reverse &
            Cracking](#Task-2-Reverse-amp-Cracking "Task 2: Reverse & Cracking")
      - [Bonus:](#Bonus "Bonus:")

</div>

<div class="toc-menu">

[Expand all](#)[Back to top](#)[Go to bottom](#)

</div>

</div>
