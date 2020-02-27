/**
 * Author: Aaron C. Smith (@clachipso, clachipso@gmail.com)
 * 
 * Copyright 2020 by Aaron C. Smith
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 **/

// This file is used for generating the lookup table that converts ADC values
// to timer capture-control register values.
#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main()
{
    printf("const PROGMEM uint16_t TIMER_TABLE[4096]\n");
    printf("{");
    for (int i = 0; i < 4096; i++)
    {
        if ((i % 8) == 0) printf("\n\t");

        double volts = (5 * i) / 4096.0;
        double freq = 65.406391325 * pow(2.0, volts);
        printf("%d, ", (uint16_t)(500000 / freq));
    }
    printf("};\n");

    return 0;
}