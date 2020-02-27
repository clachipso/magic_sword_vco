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

// This file contains the code for the Arduino Nano that is used in the Magic
// Sword VCO.

#include <stdint.h>

// The types wave tables that the oscillator can generate
enum TableType {
  TABLE_SQR,
  TABLE_QTR,
  TABLE_EIGHTH,
  TABLE_TRI,
  TABLE_SAW,
  NUM_TABLES
};

// 50% duty cycle pulse wave table, used in countless 8bit console sound 
// generators
const uint8_t SQR_TABLE[32] = {
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 25% duty cycle pulse wave table, inspired from Gameboy and NES sound chips
const uint8_t QTR_TABLE[32] = {
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 12.5% duty cycle pulse wave table, inspired from Gameboy and NES sound chips
const uint8_t EIGHTH_TABLE[32] = {
  0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Triangle wave table, inspired by the triangle channel of the NES sound 
// generator
const uint8_t TRI_TABLE[32] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
  0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};

// 4 bit saw wave table. Gameboy DMG could generate this in its wave channel
// (among many others)
const uint8_t SAW_TABLE[32] = {
  0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03,
  0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07,
  0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B,
  0x0C, 0x0C, 0x0D, 0x0D, 0x0E, 0x0E, 0x0F, 0x0F
};

// Lookup table for CCR register that times the DDS sample generation timer
const PROGMEM uint16_t TIMER_TABLE[4096]
{
  7644, 7638, 7631, 7625, 7618, 7612, 7605, 7599, 
  7592, 7586, 7580, 7573, 7567, 7560, 7554, 7548, 
  7541, 7535, 7528, 7522, 7516, 7509, 7503, 7497, 
  7490, 7484, 7478, 7471, 7465, 7459, 7452, 7446, 
  7440, 7434, 7427, 7421, 7415, 7408, 7402, 7396, 
  7390, 7383, 7377, 7371, 7365, 7358, 7352, 7346, 
  7340, 7334, 7327, 7321, 7315, 7309, 7303, 7296, 
  7290, 7284, 7278, 7272, 7266, 7259, 7253, 7247, 
  7241, 7235, 7229, 7223, 7217, 7210, 7204, 7198, 
  7192, 7186, 7180, 7174, 7168, 7162, 7156, 7150, 
  7144, 7138, 7132, 7126, 7120, 7114, 7108, 7101, 
  7095, 7089, 7083, 7077, 7072, 7066, 7060, 7054, 
  7048, 7042, 7036, 7030, 7024, 7018, 7012, 7006, 
  7000, 6994, 6988, 6982, 6976, 6971, 6965, 6959, 
  6953, 6947, 6941, 6935, 6929, 6923, 6918, 6912, 
  6906, 6900, 6894, 6888, 6883, 6877, 6871, 6865, 
  6859, 6854, 6848, 6842, 6836, 6830, 6825, 6819, 
  6813, 6807, 6802, 6796, 6790, 6784, 6779, 6773, 
  6767, 6761, 6756, 6750, 6744, 6739, 6733, 6727, 
  6721, 6716, 6710, 6704, 6699, 6693, 6687, 6682, 
  6676, 6670, 6665, 6659, 6654, 6648, 6642, 6637, 
  6631, 6625, 6620, 6614, 6609, 6603, 6597, 6592, 
  6586, 6581, 6575, 6570, 6564, 6559, 6553, 6547, 
  6542, 6536, 6531, 6525, 6520, 6514, 6509, 6503, 
  6498, 6492, 6487, 6481, 6476, 6470, 6465, 6459, 
  6454, 6448, 6443, 6438, 6432, 6427, 6421, 6416, 
  6410, 6405, 6400, 6394, 6389, 6383, 6378, 6373, 
  6367, 6362, 6356, 6351, 6346, 6340, 6335, 6330, 
  6324, 6319, 6313, 6308, 6303, 6297, 6292, 6287, 
  6281, 6276, 6271, 6266, 6260, 6255, 6250, 6244, 
  6239, 6234, 6229, 6223, 6218, 6213, 6208, 6202, 
  6197, 6192, 6187, 6181, 6176, 6171, 6166, 6160, 
  6155, 6150, 6145, 6140, 6134, 6129, 6124, 6119, 
  6114, 6109, 6103, 6098, 6093, 6088, 6083, 6078, 
  6072, 6067, 6062, 6057, 6052, 6047, 6042, 6037, 
  6031, 6026, 6021, 6016, 6011, 6006, 6001, 5996, 
  5991, 5986, 5981, 5976, 5971, 5965, 5960, 5955, 
  5950, 5945, 5940, 5935, 5930, 5925, 5920, 5915, 
  5910, 5905, 5900, 5895, 5890, 5885, 5880, 5875, 
  5870, 5865, 5860, 5855, 5850, 5846, 5841, 5836, 
  5831, 5826, 5821, 5816, 5811, 5806, 5801, 5796, 
  5791, 5786, 5782, 5777, 5772, 5767, 5762, 5757, 
  5752, 5747, 5743, 5738, 5733, 5728, 5723, 5718, 
  5714, 5709, 5704, 5699, 5694, 5689, 5685, 5680, 
  5675, 5670, 5665, 5661, 5656, 5651, 5646, 5641, 
  5637, 5632, 5627, 5622, 5618, 5613, 5608, 5603, 
  5599, 5594, 5589, 5584, 5580, 5575, 5570, 5566, 
  5561, 5556, 5551, 5547, 5542, 5537, 5533, 5528, 
  5523, 5519, 5514, 5509, 5505, 5500, 5495, 5491, 
  5486, 5481, 5477, 5472, 5468, 5463, 5458, 5454, 
  5449, 5444, 5440, 5435, 5431, 5426, 5421, 5417, 
  5412, 5408, 5403, 5399, 5394, 5389, 5385, 5380, 
  5376, 5371, 5367, 5362, 5358, 5353, 5349, 5344, 
  5340, 5335, 5330, 5326, 5321, 5317, 5312, 5308, 
  5304, 5299, 5295, 5290, 5286, 5281, 5277, 5272, 
  5268, 5263, 5259, 5254, 5250, 5245, 5241, 5237, 
  5232, 5228, 5223, 5219, 5214, 5210, 5206, 5201, 
  5197, 5192, 5188, 5184, 5179, 5175, 5171, 5166, 
  5162, 5157, 5153, 5149, 5144, 5140, 5136, 5131, 
  5127, 5123, 5118, 5114, 5110, 5105, 5101, 5097, 
  5092, 5088, 5084, 5079, 5075, 5071, 5067, 5062, 
  5058, 5054, 5049, 5045, 5041, 5037, 5032, 5028, 
  5024, 5020, 5015, 5011, 5007, 5003, 4998, 4994, 
  4990, 4986, 4982, 4977, 4973, 4969, 4965, 4961, 
  4956, 4952, 4948, 4944, 4940, 4935, 4931, 4927, 
  4923, 4919, 4915, 4910, 4906, 4902, 4898, 4894, 
  4890, 4886, 4881, 4877, 4873, 4869, 4865, 4861, 
  4857, 4853, 4848, 4844, 4840, 4836, 4832, 4828, 
  4824, 4820, 4816, 4812, 4808, 4804, 4800, 4795, 
  4791, 4787, 4783, 4779, 4775, 4771, 4767, 4763, 
  4759, 4755, 4751, 4747, 4743, 4739, 4735, 4731, 
  4727, 4723, 4719, 4715, 4711, 4707, 4703, 4699, 
  4695, 4691, 4687, 4683, 4679, 4675, 4671, 4667, 
  4663, 4659, 4656, 4652, 4648, 4644, 4640, 4636, 
  4632, 4628, 4624, 4620, 4616, 4612, 4608, 4605, 
  4601, 4597, 4593, 4589, 4585, 4581, 4577, 4574, 
  4570, 4566, 4562, 4558, 4554, 4550, 4546, 4543, 
  4539, 4535, 4531, 4527, 4523, 4520, 4516, 4512, 
  4508, 4504, 4501, 4497, 4493, 4489, 4485, 4482, 
  4478, 4474, 4470, 4466, 4463, 4459, 4455, 4451, 
  4448, 4444, 4440, 4436, 4433, 4429, 4425, 4421, 
  4418, 4414, 4410, 4406, 4403, 4399, 4395, 4391, 
  4388, 4384, 4380, 4377, 4373, 4369, 4366, 4362, 
  4358, 4354, 4351, 4347, 4343, 4340, 4336, 4332, 
  4329, 4325, 4321, 4318, 4314, 4310, 4307, 4303, 
  4300, 4296, 4292, 4289, 4285, 4281, 4278, 4274, 
  4271, 4267, 4263, 4260, 4256, 4252, 4249, 4245, 
  4242, 4238, 4235, 4231, 4227, 4224, 4220, 4217, 
  4213, 4210, 4206, 4202, 4199, 4195, 4192, 4188, 
  4185, 4181, 4178, 4174, 4171, 4167, 4163, 4160, 
  4156, 4153, 4149, 4146, 4142, 4139, 4135, 4132, 
  4128, 4125, 4121, 4118, 4114, 4111, 4107, 4104, 
  4101, 4097, 4094, 4090, 4087, 4083, 4080, 4076, 
  4073, 4069, 4066, 4063, 4059, 4056, 4052, 4049, 
  4045, 4042, 4039, 4035, 4032, 4028, 4025, 4021, 
  4018, 4015, 4011, 4008, 4005, 4001, 3998, 3994, 
  3991, 3988, 3984, 3981, 3977, 3974, 3971, 3967, 
  3964, 3961, 3957, 3954, 3951, 3947, 3944, 3941, 
  3937, 3934, 3931, 3927, 3924, 3921, 3917, 3914, 
  3911, 3907, 3904, 3901, 3898, 3894, 3891, 3888, 
  3884, 3881, 3878, 3875, 3871, 3868, 3865, 3861, 
  3858, 3855, 3852, 3848, 3845, 3842, 3839, 3835, 
  3832, 3829, 3826, 3822, 3819, 3816, 3813, 3809, 
  3806, 3803, 3800, 3797, 3793, 3790, 3787, 3784, 
  3781, 3777, 3774, 3771, 3768, 3765, 3761, 3758, 
  3755, 3752, 3749, 3746, 3742, 3739, 3736, 3733, 
  3730, 3727, 3723, 3720, 3717, 3714, 3711, 3708, 
  3705, 3701, 3698, 3695, 3692, 3689, 3686, 3683, 
  3680, 3676, 3673, 3670, 3667, 3664, 3661, 3658, 
  3655, 3652, 3649, 3645, 3642, 3639, 3636, 3633, 
  3630, 3627, 3624, 3621, 3618, 3615, 3612, 3609, 
  3606, 3603, 3600, 3596, 3593, 3590, 3587, 3584, 
  3581, 3578, 3575, 3572, 3569, 3566, 3563, 3560, 
  3557, 3554, 3551, 3548, 3545, 3542, 3539, 3536, 
  3533, 3530, 3527, 3524, 3521, 3518, 3515, 3512, 
  3509, 3506, 3503, 3500, 3497, 3494, 3491, 3489, 
  3486, 3483, 3480, 3477, 3474, 3471, 3468, 3465, 
  3462, 3459, 3456, 3453, 3450, 3447, 3445, 3442, 
  3439, 3436, 3433, 3430, 3427, 3424, 3421, 3418, 
  3416, 3413, 3410, 3407, 3404, 3401, 3398, 3395, 
  3392, 3390, 3387, 3384, 3381, 3378, 3375, 3372, 
  3370, 3367, 3364, 3361, 3358, 3355, 3353, 3350, 
  3347, 3344, 3341, 3338, 3336, 3333, 3330, 3327, 
  3324, 3321, 3319, 3316, 3313, 3310, 3307, 3305, 
  3302, 3299, 3296, 3293, 3291, 3288, 3285, 3282, 
  3280, 3277, 3274, 3271, 3268, 3266, 3263, 3260, 
  3257, 3255, 3252, 3249, 3246, 3244, 3241, 3238, 
  3235, 3233, 3230, 3227, 3225, 3222, 3219, 3216, 
  3214, 3211, 3208, 3205, 3203, 3200, 3197, 3195, 
  3192, 3189, 3187, 3184, 3181, 3178, 3176, 3173, 
  3170, 3168, 3165, 3162, 3160, 3157, 3154, 3152, 
  3149, 3146, 3144, 3141, 3138, 3136, 3133, 3130, 
  3128, 3125, 3122, 3120, 3117, 3115, 3112, 3109, 
  3107, 3104, 3101, 3099, 3096, 3094, 3091, 3088, 
  3086, 3083, 3080, 3078, 3075, 3073, 3070, 3067, 
  3065, 3062, 3060, 3057, 3055, 3052, 3049, 3047, 
  3044, 3042, 3039, 3036, 3034, 3031, 3029, 3026, 
  3024, 3021, 3019, 3016, 3013, 3011, 3008, 3006, 
  3003, 3001, 2998, 2996, 2993, 2991, 2988, 2986, 
  2983, 2980, 2978, 2975, 2973, 2970, 2968, 2965, 
  2963, 2960, 2958, 2955, 2953, 2950, 2948, 2945, 
  2943, 2940, 2938, 2935, 2933, 2930, 2928, 2925, 
  2923, 2921, 2918, 2916, 2913, 2911, 2908, 2906, 
  2903, 2901, 2898, 2896, 2893, 2891, 2889, 2886, 
  2884, 2881, 2879, 2876, 2874, 2872, 2869, 2867, 
  2864, 2862, 2859, 2857, 2855, 2852, 2850, 2847, 
  2845, 2843, 2840, 2838, 2835, 2833, 2831, 2828, 
  2826, 2823, 2821, 2819, 2816, 2814, 2811, 2809, 
  2807, 2804, 2802, 2800, 2797, 2795, 2792, 2790, 
  2788, 2785, 2783, 2781, 2778, 2776, 2774, 2771, 
  2769, 2767, 2764, 2762, 2760, 2757, 2755, 2753, 
  2750, 2748, 2746, 2743, 2741, 2739, 2736, 2734, 
  2732, 2729, 2727, 2725, 2722, 2720, 2718, 2716, 
  2713, 2711, 2709, 2706, 2704, 2702, 2700, 2697, 
  2695, 2693, 2690, 2688, 2686, 2684, 2681, 2679, 
  2677, 2674, 2672, 2670, 2668, 2665, 2663, 2661, 
  2659, 2656, 2654, 2652, 2650, 2647, 2645, 2643, 
  2641, 2639, 2636, 2634, 2632, 2630, 2627, 2625, 
  2623, 2621, 2618, 2616, 2614, 2612, 2610, 2607, 
  2605, 2603, 2601, 2599, 2596, 2594, 2592, 2590, 
  2588, 2585, 2583, 2581, 2579, 2577, 2575, 2572, 
  2570, 2568, 2566, 2564, 2562, 2559, 2557, 2555, 
  2553, 2551, 2549, 2546, 2544, 2542, 2540, 2538, 
  2536, 2533, 2531, 2529, 2527, 2525, 2523, 2521, 
  2519, 2516, 2514, 2512, 2510, 2508, 2506, 2504, 
  2502, 2499, 2497, 2495, 2493, 2491, 2489, 2487, 
  2485, 2483, 2480, 2478, 2476, 2474, 2472, 2470, 
  2468, 2466, 2464, 2462, 2460, 2457, 2455, 2453, 
  2451, 2449, 2447, 2445, 2443, 2441, 2439, 2437, 
  2435, 2433, 2431, 2429, 2426, 2424, 2422, 2420, 
  2418, 2416, 2414, 2412, 2410, 2408, 2406, 2404, 
  2402, 2400, 2398, 2396, 2394, 2392, 2390, 2388, 
  2386, 2384, 2382, 2380, 2378, 2376, 2374, 2372, 
  2370, 2368, 2366, 2364, 2362, 2360, 2358, 2356, 
  2354, 2352, 2350, 2348, 2346, 2344, 2342, 2340, 
  2338, 2336, 2334, 2332, 2330, 2328, 2326, 2324, 
  2322, 2320, 2318, 2316, 2314, 2312, 2310, 2308, 
  2306, 2304, 2302, 2300, 2299, 2297, 2295, 2293, 
  2291, 2289, 2287, 2285, 2283, 2281, 2279, 2277, 
  2275, 2273, 2271, 2270, 2268, 2266, 2264, 2262, 
  2260, 2258, 2256, 2254, 2252, 2250, 2249, 2247, 
  2245, 2243, 2241, 2239, 2237, 2235, 2233, 2231, 
  2230, 2228, 2226, 2224, 2222, 2220, 2218, 2216, 
  2215, 2213, 2211, 2209, 2207, 2205, 2203, 2201, 
  2200, 2198, 2196, 2194, 2192, 2190, 2188, 2187, 
  2185, 2183, 2181, 2179, 2177, 2176, 2174, 2172, 
  2170, 2168, 2166, 2164, 2163, 2161, 2159, 2157, 
  2155, 2154, 2152, 2150, 2148, 2146, 2144, 2143, 
  2141, 2139, 2137, 2135, 2134, 2132, 2130, 2128, 
  2126, 2125, 2123, 2121, 2119, 2117, 2116, 2114, 
  2112, 2110, 2108, 2107, 2105, 2103, 2101, 2100, 
  2098, 2096, 2094, 2092, 2091, 2089, 2087, 2085, 
  2084, 2082, 2080, 2078, 2077, 2075, 2073, 2071, 
  2070, 2068, 2066, 2064, 2063, 2061, 2059, 2057, 
  2056, 2054, 2052, 2050, 2049, 2047, 2045, 2043, 
  2042, 2040, 2038, 2037, 2035, 2033, 2031, 2030, 
  2028, 2026, 2024, 2023, 2021, 2019, 2018, 2016, 
  2014, 2013, 2011, 2009, 2007, 2006, 2004, 2002, 
  2001, 1999, 1997, 1996, 1994, 1992, 1991, 1989, 
  1987, 1985, 1984, 1982, 1980, 1979, 1977, 1975, 
  1974, 1972, 1970, 1969, 1967, 1965, 1964, 1962, 
  1960, 1959, 1957, 1955, 1954, 1952, 1950, 1949, 
  1947, 1946, 1944, 1942, 1941, 1939, 1937, 1936, 
  1934, 1932, 1931, 1929, 1928, 1926, 1924, 1923, 
  1921, 1919, 1918, 1916, 1915, 1913, 1911, 1910, 
  1908, 1906, 1905, 1903, 1902, 1900, 1898, 1897, 
  1895, 1894, 1892, 1890, 1889, 1887, 1886, 1884, 
  1882, 1881, 1879, 1878, 1876, 1874, 1873, 1871, 
  1870, 1868, 1867, 1865, 1863, 1862, 1860, 1859, 
  1857, 1855, 1854, 1852, 1851, 1849, 1848, 1846, 
  1845, 1843, 1841, 1840, 1838, 1837, 1835, 1834, 
  1832, 1831, 1829, 1827, 1826, 1824, 1823, 1821, 
  1820, 1818, 1817, 1815, 1814, 1812, 1811, 1809, 
  1807, 1806, 1804, 1803, 1801, 1800, 1798, 1797, 
  1795, 1794, 1792, 1791, 1789, 1788, 1786, 1785, 
  1783, 1782, 1780, 1779, 1777, 1776, 1774, 1773, 
  1771, 1770, 1768, 1767, 1765, 1764, 1762, 1761, 
  1759, 1758, 1756, 1755, 1753, 1752, 1750, 1749, 
  1747, 1746, 1744, 1743, 1741, 1740, 1738, 1737, 
  1735, 1734, 1733, 1731, 1730, 1728, 1727, 1725, 
  1724, 1722, 1721, 1719, 1718, 1716, 1715, 1714, 
  1712, 1711, 1709, 1708, 1706, 1705, 1703, 1702, 
  1701, 1699, 1698, 1696, 1695, 1693, 1692, 1691, 
  1689, 1688, 1686, 1685, 1683, 1682, 1681, 1679, 
  1678, 1676, 1675, 1673, 1672, 1671, 1669, 1668, 
  1666, 1665, 1664, 1662, 1661, 1659, 1658, 1657, 
  1655, 1654, 1652, 1651, 1650, 1648, 1647, 1645, 
  1644, 1643, 1641, 1640, 1638, 1637, 1636, 1634, 
  1633, 1632, 1630, 1629, 1627, 1626, 1625, 1623, 
  1622, 1620, 1619, 1618, 1616, 1615, 1614, 1612, 
  1611, 1610, 1608, 1607, 1605, 1604, 1603, 1601, 
  1600, 1599, 1597, 1596, 1595, 1593, 1592, 1591, 
  1589, 1588, 1587, 1585, 1584, 1583, 1581, 1580, 
  1579, 1577, 1576, 1575, 1573, 1572, 1571, 1569, 
  1568, 1567, 1565, 1564, 1563, 1561, 1560, 1559, 
  1557, 1556, 1555, 1553, 1552, 1551, 1549, 1548, 
  1547, 1545, 1544, 1543, 1542, 1540, 1539, 1538, 
  1536, 1535, 1534, 1532, 1531, 1530, 1529, 1527, 
  1526, 1525, 1523, 1522, 1521, 1520, 1518, 1517, 
  1516, 1514, 1513, 1512, 1511, 1509, 1508, 1507, 
  1505, 1504, 1503, 1502, 1500, 1499, 1498, 1497, 
  1495, 1494, 1493, 1491, 1490, 1489, 1488, 1486, 
  1485, 1484, 1483, 1481, 1480, 1479, 1478, 1476, 
  1475, 1474, 1473, 1471, 1470, 1469, 1468, 1466, 
  1465, 1464, 1463, 1462, 1460, 1459, 1458, 1457, 
  1455, 1454, 1453, 1452, 1450, 1449, 1448, 1447, 
  1446, 1444, 1443, 1442, 1441, 1439, 1438, 1437, 
  1436, 1435, 1433, 1432, 1431, 1430, 1428, 1427, 
  1426, 1425, 1424, 1422, 1421, 1420, 1419, 1418, 
  1416, 1415, 1414, 1413, 1412, 1410, 1409, 1408, 
  1407, 1406, 1405, 1403, 1402, 1401, 1400, 1399, 
  1397, 1396, 1395, 1394, 1393, 1391, 1390, 1389, 
  1388, 1387, 1386, 1384, 1383, 1382, 1381, 1380, 
  1379, 1377, 1376, 1375, 1374, 1373, 1372, 1370, 
  1369, 1368, 1367, 1366, 1365, 1364, 1362, 1361, 
  1360, 1359, 1358, 1357, 1355, 1354, 1353, 1352, 
  1351, 1350, 1349, 1347, 1346, 1345, 1344, 1343, 
  1342, 1341, 1339, 1338, 1337, 1336, 1335, 1334, 
  1333, 1332, 1330, 1329, 1328, 1327, 1326, 1325, 
  1324, 1323, 1321, 1320, 1319, 1318, 1317, 1316, 
  1315, 1314, 1313, 1311, 1310, 1309, 1308, 1307, 
  1306, 1305, 1304, 1303, 1301, 1300, 1299, 1298, 
  1297, 1296, 1295, 1294, 1293, 1292, 1291, 1289, 
  1288, 1287, 1286, 1285, 1284, 1283, 1282, 1281, 
  1280, 1279, 1277, 1276, 1275, 1274, 1273, 1272, 
  1271, 1270, 1269, 1268, 1267, 1266, 1265, 1263, 
  1262, 1261, 1260, 1259, 1258, 1257, 1256, 1255, 
  1254, 1253, 1252, 1251, 1250, 1249, 1248, 1246, 
  1245, 1244, 1243, 1242, 1241, 1240, 1239, 1238, 
  1237, 1236, 1235, 1234, 1233, 1232, 1231, 1230, 
  1229, 1228, 1227, 1226, 1225, 1223, 1222, 1221, 
  1220, 1219, 1218, 1217, 1216, 1215, 1214, 1213, 
  1212, 1211, 1210, 1209, 1208, 1207, 1206, 1205, 
  1204, 1203, 1202, 1201, 1200, 1199, 1198, 1197, 
  1196, 1195, 1194, 1193, 1192, 1191, 1190, 1189, 
  1188, 1187, 1186, 1185, 1184, 1183, 1182, 1181, 
  1180, 1179, 1178, 1177, 1176, 1175, 1174, 1173, 
  1172, 1171, 1170, 1169, 1168, 1167, 1166, 1165, 
  1164, 1163, 1162, 1161, 1160, 1159, 1158, 1157, 
  1156, 1155, 1154, 1153, 1152, 1151, 1150, 1149, 
  1148, 1147, 1146, 1145, 1144, 1143, 1142, 1141, 
  1140, 1140, 1139, 1138, 1137, 1136, 1135, 1134, 
  1133, 1132, 1131, 1130, 1129, 1128, 1127, 1126, 
  1125, 1124, 1123, 1122, 1121, 1120, 1119, 1118, 
  1118, 1117, 1116, 1115, 1114, 1113, 1112, 1111, 
  1110, 1109, 1108, 1107, 1106, 1105, 1104, 1103, 
  1103, 1102, 1101, 1100, 1099, 1098, 1097, 1096, 
  1095, 1094, 1093, 1092, 1091, 1090, 1090, 1089, 
  1088, 1087, 1086, 1085, 1084, 1083, 1082, 1081, 
  1080, 1079, 1079, 1078, 1077, 1076, 1075, 1074, 
  1073, 1072, 1071, 1070, 1069, 1069, 1068, 1067, 
  1066, 1065, 1064, 1063, 1062, 1061, 1060, 1060, 
  1059, 1058, 1057, 1056, 1055, 1054, 1053, 1052, 
  1051, 1051, 1050, 1049, 1048, 1047, 1046, 1045, 
  1044, 1043, 1043, 1042, 1041, 1040, 1039, 1038, 
  1037, 1036, 1036, 1035, 1034, 1033, 1032, 1031, 
  1030, 1029, 1029, 1028, 1027, 1026, 1025, 1024, 
  1023, 1023, 1022, 1021, 1020, 1019, 1018, 1017, 
  1016, 1016, 1015, 1014, 1013, 1012, 1011, 1010, 
  1010, 1009, 1008, 1007, 1006, 1005, 1004, 1004, 
  1003, 1002, 1001, 1000, 999, 999, 998, 997, 
  996, 995, 994, 993, 993, 992, 991, 990, 
  989, 988, 988, 987, 986, 985, 984, 983, 
  983, 982, 981, 980, 979, 978, 978, 977, 
  976, 975, 974, 974, 973, 972, 971, 970, 
  969, 969, 968, 967, 966, 965, 964, 964, 
  963, 962, 961, 960, 960, 959, 958, 957, 
  956, 956, 955, 954, 953, 952, 952, 951, 
  950, 949, 948, 947, 947, 946, 945, 944, 
  943, 943, 942, 941, 940, 940, 939, 938, 
  937, 936, 936, 935, 934, 933, 932, 932, 
  931, 930, 929, 928, 928, 927, 926, 925, 
  925, 924, 923, 922, 921, 921, 920, 919, 
  918, 917, 917, 916, 915, 914, 914, 913, 
  912, 911, 911, 910, 909, 908, 907, 907, 
  906, 905, 904, 904, 903, 902, 901, 901, 
  900, 899, 898, 898, 897, 896, 895, 894, 
  894, 893, 892, 891, 891, 890, 889, 888, 
  888, 887, 886, 885, 885, 884, 883, 882, 
  882, 881, 880, 879, 879, 878, 877, 876, 
  876, 875, 874, 874, 873, 872, 871, 871, 
  870, 869, 868, 868, 867, 866, 865, 865, 
  864, 863, 863, 862, 861, 860, 860, 859, 
  858, 857, 857, 856, 855, 855, 854, 853, 
  852, 852, 851, 850, 849, 849, 848, 847, 
  847, 846, 845, 844, 844, 843, 842, 842, 
  841, 840, 839, 839, 838, 837, 837, 836, 
  835, 834, 834, 833, 832, 832, 831, 830, 
  830, 829, 828, 827, 827, 826, 825, 825, 
  824, 823, 823, 822, 821, 820, 820, 819, 
  818, 818, 817, 816, 816, 815, 814, 814, 
  813, 812, 812, 811, 810, 809, 809, 808, 
  807, 807, 806, 805, 805, 804, 803, 803, 
  802, 801, 801, 800, 799, 799, 798, 797, 
  797, 796, 795, 795, 794, 793, 792, 792, 
  791, 790, 790, 789, 788, 788, 787, 786, 
  786, 785, 784, 784, 783, 782, 782, 781, 
  781, 780, 779, 779, 778, 777, 777, 776, 
  775, 775, 774, 773, 773, 772, 771, 771, 
  770, 769, 769, 768, 767, 767, 766, 765, 
  765, 764, 764, 763, 762, 762, 761, 760, 
  760, 759, 758, 758, 757, 756, 756, 755, 
  755, 754, 753, 753, 752, 751, 751, 750, 
  749, 749, 748, 748, 747, 746, 746, 745, 
  744, 744, 743, 742, 742, 741, 741, 740, 
  739, 739, 738, 737, 737, 736, 736, 735, 
  734, 734, 733, 732, 732, 731, 731, 730, 
  729, 729, 728, 728, 727, 726, 726, 725, 
  724, 724, 723, 723, 722, 721, 721, 720, 
  720, 719, 718, 718, 717, 717, 716, 715, 
  715, 714, 714, 713, 712, 712, 711, 710, 
  710, 709, 709, 708, 707, 707, 706, 706, 
  705, 705, 704, 703, 703, 702, 702, 701, 
  700, 700, 699, 699, 698, 697, 697, 696, 
  696, 695, 694, 694, 693, 693, 692, 692, 
  691, 690, 690, 689, 689, 688, 687, 687, 
  686, 686, 685, 685, 684, 683, 683, 682, 
  682, 681, 680, 680, 679, 679, 678, 678, 
  677, 676, 676, 675, 675, 674, 674, 673, 
  672, 672, 671, 671, 670, 670, 669, 668, 
  668, 667, 667, 666, 666, 665, 665, 664, 
  663, 663, 662, 662, 661, 661, 660, 659, 
  659, 658, 658, 657, 657, 656, 656, 655, 
  654, 654, 653, 653, 652, 652, 651, 651, 
  650, 650, 649, 648, 648, 647, 647, 646, 
  646, 645, 645, 644, 643, 643, 642, 642, 
  641, 641, 640, 640, 639, 639, 638, 638, 
  637, 636, 636, 635, 635, 634, 634, 633, 
  633, 632, 632, 631, 631, 630, 629, 629, 
  628, 628, 627, 627, 626, 626, 625, 625, 
  624, 624, 623, 623, 622, 622, 621, 620, 
  620, 619, 619, 618, 618, 617, 617, 616, 
  616, 615, 615, 614, 614, 613, 613, 612, 
  612, 611, 611, 610, 610, 609, 609, 608, 
  607, 607, 606, 606, 605, 605, 604, 604, 
  603, 603, 602, 602, 601, 601, 600, 600, 
  599, 599, 598, 598, 597, 597, 596, 596, 
  595, 595, 594, 594, 593, 593, 592, 592, 
  591, 591, 590, 590, 589, 589, 588, 588, 
  587, 587, 586, 586, 585, 585, 584, 584, 
  583, 583, 582, 582, 581, 581, 580, 580, 
  579, 579, 578, 578, 577, 577, 576, 576, 
  575, 575, 574, 574, 573, 573, 573, 572, 
  572, 571, 571, 570, 570, 569, 569, 568, 
  568, 567, 567, 566, 566, 565, 565, 564, 
  564, 563, 563, 562, 562, 561, 561, 561, 
  560, 560, 559, 559, 558, 558, 557, 557, 
  556, 556, 555, 555, 554, 554, 553, 553, 
  553, 552, 552, 551, 551, 550, 550, 549, 
  549, 548, 548, 547, 547, 546, 546, 546, 
  545, 545, 544, 544, 543, 543, 542, 542, 
  541, 541, 540, 540, 540, 539, 539, 538, 
  538, 537, 537, 536, 536, 535, 535, 535, 
  534, 534, 533, 533, 532, 532, 531, 531, 
  530, 530, 530, 529, 529, 528, 528, 527, 
  527, 526, 526, 526, 525, 525, 524, 524, 
  523, 523, 522, 522, 522, 521, 521, 520, 
  520, 519, 519, 519, 518, 518, 517, 517, 
  516, 516, 515, 515, 515, 514, 514, 513, 
  513, 512, 512, 512, 511, 511, 510, 510, 
  509, 509, 508, 508, 508, 507, 507, 506, 
  506, 505, 505, 505, 504, 504, 503, 503, 
  503, 502, 502, 501, 501, 500, 500, 500, 
  499, 499, 498, 498, 497, 497, 497, 496, 
  496, 495, 495, 494, 494, 494, 493, 493, 
  492, 492, 492, 491, 491, 490, 490, 489, 
  489, 489, 488, 488, 487, 487, 487, 486, 
  486, 485, 485, 485, 484, 484, 483, 483, 
  482, 482, 482, 481, 481, 480, 480, 480, 
  479, 479, 478, 478, 478, 477, 477, 476, 
  476, 476, 475, 475, 474, 474, 474, 473, 
  473, 472, 472, 472, 471, 471, 470, 470, 
  470, 469, 469, 468, 468, 468, 467, 467, 
  466, 466, 466, 465, 465, 464, 464, 464, 
  463, 463, 462, 462, 462, 461, 461, 461, 
  460, 460, 459, 459, 459, 458, 458, 457, 
  457, 457, 456, 456, 455, 455, 455, 454, 
  454, 454, 453, 453, 452, 452, 452, 451, 
  451, 450, 450, 450, 449, 449, 449, 448, 
  448, 447, 447, 447, 446, 446, 446, 445, 
  445, 444, 444, 444, 443, 443, 443, 442, 
  442, 441, 441, 441, 440, 440, 440, 439, 
  439, 438, 438, 438, 437, 437, 437, 436, 
  436, 435, 435, 435, 434, 434, 434, 433, 
  433, 433, 432, 432, 431, 431, 431, 430, 
  430, 430, 429, 429, 429, 428, 428, 427, 
  427, 427, 426, 426, 426, 425, 425, 425, 
  424, 424, 423, 423, 423, 422, 422, 422, 
  421, 421, 421, 420, 420, 420, 419, 419, 
  418, 418, 418, 417, 417, 417, 416, 416, 
  416, 415, 415, 415, 414, 414, 414, 413, 
  413, 413, 412, 412, 411, 411, 411, 410, 
  410, 410, 409, 409, 409, 408, 408, 408, 
  407, 407, 407, 406, 406, 406, 405, 405, 
  405, 404, 404, 404, 403, 403, 402, 402, 
  402, 401, 401, 401, 400, 400, 400, 399, 
  399, 399, 398, 398, 398, 397, 397, 397, 
  396, 396, 396, 395, 395, 395, 394, 394, 
  394, 393, 393, 393, 392, 392, 392, 391, 
  391, 391, 390, 390, 390, 389, 389, 389, 
  388, 388, 388, 387, 387, 387, 386, 386, 
  386, 385, 385, 385, 384, 384, 384, 384, 
  383, 383, 383, 382, 382, 382, 381, 381, 
  381, 380, 380, 380, 379, 379, 379, 378, 
  378, 378, 377, 377, 377, 376, 376, 376, 
  375, 375, 375, 375, 374, 374, 374, 373, 
  373, 373, 372, 372, 372, 371, 371, 371, 
  370, 370, 370, 369, 369, 369, 369, 368, 
  368, 368, 367, 367, 367, 366, 366, 366, 
  365, 365, 365, 365, 364, 364, 364, 363, 
  363, 363, 362, 362, 362, 361, 361, 361, 
  361, 360, 360, 360, 359, 359, 359, 358, 
  358, 358, 357, 357, 357, 357, 356, 356, 
  356, 355, 355, 355, 354, 354, 354, 354, 
  353, 353, 353, 352, 352, 352, 351, 351, 
  351, 351, 350, 350, 350, 349, 349, 349, 
  349, 348, 348, 348, 347, 347, 347, 346, 
  346, 346, 346, 345, 345, 345, 344, 344, 
  344, 344, 343, 343, 343, 342, 342, 342, 
  341, 341, 341, 341, 340, 340, 340, 339, 
  339, 339, 339, 338, 338, 338, 337, 337, 
  337, 337, 336, 336, 336, 335, 335, 335, 
  335, 334, 334, 334, 333, 333, 333, 333, 
  332, 332, 332, 332, 331, 331, 331, 330, 
  330, 330, 330, 329, 329, 329, 328, 328, 
  328, 328, 327, 327, 327, 326, 326, 326, 
  326, 325, 325, 325, 325, 324, 324, 324, 
  323, 323, 323, 323, 322, 322, 322, 322, 
  321, 321, 321, 320, 320, 320, 320, 319, 
  319, 319, 319, 318, 318, 318, 317, 317, 
  317, 317, 316, 316, 316, 316, 315, 315, 
  315, 315, 314, 314, 314, 313, 313, 313, 
  313, 312, 312, 312, 312, 311, 311, 311, 
  311, 310, 310, 310, 310, 309, 309, 309, 
  308, 308, 308, 308, 307, 307, 307, 307, 
  306, 306, 306, 306, 305, 305, 305, 305, 
  304, 304, 304, 304, 303, 303, 303, 303, 
  302, 302, 302, 301, 301, 301, 301, 300, 
  300, 300, 300, 299, 299, 299, 299, 298, 
  298, 298, 298, 297, 297, 297, 297, 296, 
  296, 296, 296, 295, 295, 295, 295, 294, 
  294, 294, 294, 293, 293, 293, 293, 292, 
  292, 292, 292, 291, 291, 291, 291, 290, 
  290, 290, 290, 289, 289, 289, 289, 288, 
  288, 288, 288, 288, 287, 287, 287, 287, 
  286, 286, 286, 286, 285, 285, 285, 285, 
  284, 284, 284, 284, 283, 283, 283, 283, 
  282, 282, 282, 282, 281, 281, 281, 281, 
  281, 280, 280, 280, 280, 279, 279, 279, 
  279, 278, 278, 278, 278, 277, 277, 277, 
  277, 277, 276, 276, 276, 276, 275, 275, 
  275, 275, 274, 274, 274, 274, 273, 273, 
  273, 273, 273, 272, 272, 272, 272, 271, 
  271, 271, 271, 270, 270, 270, 270, 270, 
  269, 269, 269, 269, 268, 268, 268, 268, 
  268, 267, 267, 267, 267, 266, 266, 266, 
  266, 265, 265, 265, 265, 265, 264, 264, 
  264, 264, 263, 263, 263, 263, 263, 262, 
  262, 262, 262, 261, 261, 261, 261, 261, 
  260, 260, 260, 260, 259, 259, 259, 259, 
  259, 258, 258, 258, 258, 258, 257, 257, 
  257, 257, 256, 256, 256, 256, 256, 255, 
  255, 255, 255, 254, 254, 254, 254, 254, 
  253, 253, 253, 253, 253, 252, 252, 252, 
  252, 251, 251, 251, 251, 251, 250, 250, 
  250, 250, 250, 249, 249, 249, 249, 249, 
  248, 248, 248, 248, 247, 247, 247, 247, 
  247, 246, 246, 246, 246, 246, 245, 245, 
  245, 245, 245, 244, 244, 244, 244, 243, 
  243, 243, 243, 243, 242, 242, 242, 242, 
  242, 241, 241, 241, 241, 241, 240, 240, 
  240, 240, 240, 239, 239, 239, 239, 239 
};


uint16_t curr_adc_val = 0;
uint16_t timer_val = 500000 / 65;
uint8_t phase_acc = 0;
uint8_t table_type = TABLE_TRI;

bool prev_btn_state = false;

void setup() {
  // Set up DAC related functionality
  DDRB |= 0x0F; // PB0-PB3 are our DAC pins 
  TCCR1A = 0;
  TCCR1B = (1 << WGM12); // CTC Mode, OCR1A is top
  TCCR1B |= (1 << CS10); // Divide clock by 1 for full 16MHz master clock
  TIMSK1 = (1 << OCIE1A); // Enable timer interrupt
  OCR1A = 500000 / 65; // Initial value for 65Hz (lowest C in our range)

  // Initialize waveform selection pushbutton. We'll use normal Arduino
  // stuff here since it isn't performance sensitive
  pinMode(A1, INPUT_PULLUP);

  // Initialize pins for RGB LED to indicate waveform
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  // Enable interrupts to start synthesis
  sei();
}

void loop() {
  /**
   * Read in the next CV input value to determine the frequency of the 
   * oscillator. Note that we oversample by 16. The purpose of this is to reduce 
   * the amount of frequency change that 1 LSB can cause. This avoids oscillator 
   * detuning from minor noise on the ADC input. Note that this makes our 
   * effective sample rate low, and audio rate FM is probably going to do some 
   * weird stuff. Vibrato effects from an LFO should work though.
   **/
  uint16_t adc_avg = 0;
  for (int i = 0; i < 16; i++) {
    adc_avg += analogRead(A0);
  }
  adc_avg >>= 2;

  if (adc_avg != curr_adc_val) {
    curr_adc_val = adc_avg;
    timer_val = pgm_read_word_near(TIMER_TABLE + curr_adc_val);
  }

  // We now have a nearly 12 bit sample. Our range is limited from 0-4092, not 4095
  // The reason is that: 1023 * 16 / 4 = 4092.

  // Check pushbutton for waveform select input. We don't use debounce, but 
  // maybe we should?
  uint8_t curr_btn_state = 0;
  if (digitalRead(A1) == HIGH) curr_btn_state = 1;
  if (curr_btn_state == 0 && prev_btn_state == 1) {
    if ((table_type + 1) == NUM_TABLES) table_type = 0;
    else table_type++;
  }
  prev_btn_state = curr_btn_state;

  // Now update RGB LED based on waveform
  switch (table_type) {
    case TABLE_SQR:
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      break;
    case TABLE_QTR:
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      break;
     case TABLE_EIGHTH:
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      digitalWrite(4, LOW);
      break;
     case TABLE_TRI:
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      break;
     case TABLE_SAW:
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      break;
  }
}

// Here's where we output the next value to our DAC. We use DDS technique to 
// avoid aliasing, while still getting that sweet quantization noise from the 
// 8bit console days 
ISR(TIMER1_COMPA_vect) {
  const uint8_t *table = SQR_TABLE;
  if (table_type == TABLE_QTR) table = QTR_TABLE;
  else if (table_type == TABLE_EIGHTH) table = EIGHTH_TABLE;
  else if (table_type == TABLE_TRI) table = TRI_TABLE;
  else if (table_type == TABLE_SAW) table = SAW_TABLE;
  
  PORTB = table[phase_acc];
  
  phase_acc++;
  phase_acc &= 0x1F;
  OCR1A = timer_val;
}