#!/bin/bash
for FILE in $(ls src_w/*.*)
do
	sed -i "s/<cumt\/cumt.h>/\"cumt\/cumt.h\"/; s/<shitrndr.h>/\"cumt\/shitrndr.h\"/" $FILE
	sed -i "s/\#include\s<cumt\/.*>//g" $FILE

	sed -i "s/<fizzyx\/fizzyx.h>/\"fizzyx\/fizzyx.h\"/" $FILE
	sed -i "s/\#include\s<fizzyx\/.*>//" $FILE

	sed -i "s/M_PI_2f32/M_PIf32\/2.f/g ; s/M_PIf32/3.1415926535f/g" $FILE
done
for FILE in $(ls src_w/cumt/*.*)
do
	sed -i "s/<cumt\/cumt.h>/\"cumt\/cumt.h\"/; s/<shitrndr.h>/\"shitrndr.h\"/" $FILE
	sed -i "s/M_PI_2f32/M_PIf32\/2.f/g ; s/M_PIf32/3.1415926535f/g" $FILE
done
for FILE in $(ls src_w/fizzyx/*.*)
do
	sed -i "s/<cumt\/cumt.h>/\"..\/cumt\/cumt.h\"/; s/<shitrndr.h>/\"..\/cumt\/shitrndr.h\"/" $FILE
	sed -i "s/\#include\s<cumt\/.*>//g" $FILE

	sed -i "s/M_PI_2f32/M_PIf32\/2.f/g ; s/M_PIf32/3.1415926535f/g" $FILE
done
