	// 1114.1.0
	 #pragma once
const uint32_t simplesort_comp[] = {
	0x07230203,0x00010000,0x0008000b,0x000000b0,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0006000f,0x00000005,0x00000004,0x6e69616d,0x00000000,0x00000011,0x00060010,0x00000004,
	0x00000011,0x00000400,0x00000001,0x00000001,0x00030003,0x00000002,0x000001c2,0x00040005,
	0x00000004,0x6e69616d,0x00000000,0x00050005,0x00000006,0x61426f64,0x65697272,0x00287372,
	0x00070005,0x0000000e,0x65646e69,0x546e4978,0x61657268,0x6f724764,0x00007075,0x00080005,
	0x00000011,0x4c5f6c67,0x6c61636f,0x6f766e49,0x69746163,0x44496e6f,0x00000000,0x00040005,
	0x00000016,0x65646e69,0x00000078,0x00050005,0x0000001f,0x61746144,0x657a6953,0x00000000,
	0x00060006,0x0000001f,0x00000000,0x61746164,0x657a6953,0x00000000,0x00060006,0x0000001f,
	0x00000001,0x61746164,0x657a6953,0x00000032,0x00030005,0x00000021,0x00000000,0x00040005,
	0x0000002f,0x756c6176,0x00317365,0x00040005,0x00000032,0x61746144,0x00006e49,0x00050006,
	0x00000032,0x00000000,0x61746164,0x00006e49,0x00030005,0x00000034,0x00000000,0x00040005,
	0x0000003f,0x65646e69,0x00000078,0x00040005,0x0000004c,0x34646e69,0x00000000,0x00040005,
	0x00000057,0x756c6176,0x00000065,0x00040005,0x00000060,0x756c6176,0x00007365,0x00050005,
	0x00000066,0x6c616d73,0x7372656c,0x00000000,0x00040005,0x00000068,0x65646e69,0x00736578,
	0x00040005,0x0000006d,0x706f6f6c,0x00000000,0x00050005,0x00000078,0x706f6f6c,0x756c6156,
	0x00007365,0x00040005,0x0000007c,0x73636564,0x00000000,0x00040005,0x00000087,0x66666964,
	0x00000073,0x00050005,0x00000099,0x74697277,0x646e4965,0x00007865,0x00040005,0x000000a7,
	0x61746144,0x0074754f,0x00050006,0x000000a7,0x00000000,0x61746164,0x0074754f,0x00030005,
	0x000000a9,0x00000000,0x00040047,0x00000011,0x0000000b,0x0000001b,0x00050048,0x0000001f,
	0x00000000,0x00000023,0x00000000,0x00050048,0x0000001f,0x00000001,0x00000023,0x00000004,
	0x00030047,0x0000001f,0x00000002,0x00040047,0x00000021,0x00000022,0x00000002,0x00040047,
	0x00000021,0x00000021,0x00000000,0x00040047,0x00000031,0x00000006,0x00000010,0x00040048,
	0x00000032,0x00000000,0x00000013,0x00050048,0x00000032,0x00000000,0x00000023,0x00000000,
	0x00030047,0x00000032,0x00000003,0x00040047,0x00000034,0x00000022,0x00000001,0x00040047,
	0x00000034,0x00000021,0x00000000,0x00040047,0x000000a6,0x00000006,0x00000004,0x00040048,
	0x000000a7,0x00000000,0x00000013,0x00050048,0x000000a7,0x00000000,0x00000023,0x00000000,
	0x00030047,0x000000a7,0x00000003,0x00040047,0x000000a9,0x00000022,0x00000001,0x00040047,
	0x000000a9,0x00000021,0x00000001,0x00040047,0x000000af,0x0000000b,0x00000019,0x00020013,
	0x00000002,0x00030021,0x00000003,0x00000002,0x00040015,0x00000008,0x00000020,0x00000000,
	0x0004002b,0x00000008,0x00000009,0x00000001,0x0004002b,0x00000008,0x0000000a,0x00000108,
	0x0004002b,0x00000008,0x0000000b,0x00000002,0x0004002b,0x00000008,0x0000000c,0x00000d48,
	0x00040020,0x0000000d,0x00000007,0x00000008,0x00040017,0x0000000f,0x00000008,0x00000003,
	0x00040020,0x00000010,0x00000001,0x0000000f,0x0004003b,0x00000010,0x00000011,0x00000001,
	0x0004002b,0x00000008,0x00000012,0x00000000,0x00040020,0x00000013,0x00000001,0x00000008,
	0x00040015,0x0000001e,0x00000020,0x00000001,0x0004001e,0x0000001f,0x0000001e,0x0000001e,
	0x00040020,0x00000020,0x00000002,0x0000001f,0x0004003b,0x00000020,0x00000021,0x00000002,
	0x0004002b,0x0000001e,0x00000022,0x00000000,0x00040020,0x00000023,0x00000002,0x0000001e,
	0x0004002b,0x0000001e,0x00000026,0x00000004,0x00020014,0x00000029,0x00040017,0x0000002b,
	0x0000001e,0x00000004,0x0004002b,0x00000008,0x0000002c,0x00000200,0x0004001c,0x0000002d,
	0x0000002b,0x0000002c,0x00040020,0x0000002e,0x00000004,0x0000002d,0x0004003b,0x0000002e,
	0x0000002f,0x00000004,0x0003001d,0x00000031,0x0000002b,0x0003001e,0x00000032,0x00000031,
	0x00040020,0x00000033,0x00000002,0x00000032,0x0004003b,0x00000033,0x00000034,0x00000002,
	0x00040020,0x00000036,0x00000002,0x0000002b,0x00040020,0x00000039,0x00000004,0x0000002b,
	0x0004002b,0x00000008,0x0000003b,0x00000400,0x00040020,0x0000004b,0x00000007,0x0000002b,
	0x00040020,0x00000056,0x00000007,0x0000001e,0x0004002b,0x00000008,0x00000059,0x00000004,
	0x00040020,0x0000005d,0x00000004,0x0000001e,0x0007002c,0x0000002b,0x00000067,0x00000022,
	0x00000022,0x00000022,0x00000022,0x0004002b,0x0000001e,0x00000069,0x00000001,0x0004002b,
	0x0000001e,0x0000006a,0x00000002,0x0004002b,0x0000001e,0x0000006b,0x00000003,0x0007002c,
	0x0000002b,0x0000006c,0x00000022,0x00000069,0x0000006a,0x0000006b,0x0004002b,0x0000001e,
	0x00000081,0x0000001f,0x0007002c,0x0000002b,0x00000082,0x00000081,0x00000081,0x00000081,
	0x00000081,0x0007002c,0x0000002b,0x00000085,0x00000069,0x00000069,0x00000069,0x00000069,
	0x0007002c,0x0000002b,0x00000094,0x00000026,0x00000026,0x00000026,0x00000026,0x0004002b,
	0x00000008,0x000000a2,0x00000003,0x0003001d,0x000000a6,0x0000001e,0x0003001e,0x000000a7,
	0x000000a6,0x00040020,0x000000a8,0x00000002,0x000000a7,0x0004003b,0x000000a8,0x000000a9,
	0x00000002,0x0006002c,0x0000000f,0x000000af,0x0000003b,0x00000009,0x00000009,0x00050036,
	0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003b,0x0000000d,
	0x0000000e,0x00000007,0x0004003b,0x0000000d,0x00000016,0x00000007,0x0004003b,0x0000000d,
	0x0000003f,0x00000007,0x0004003b,0x0000004b,0x0000004c,0x00000007,0x0004003b,0x00000056,
	0x00000057,0x00000007,0x0004003b,0x0000004b,0x00000060,0x00000007,0x0004003b,0x0000004b,
	0x00000066,0x00000007,0x0004003b,0x0000004b,0x00000068,0x00000007,0x0004003b,0x00000056,
	0x0000006d,0x00000007,0x0004003b,0x0000004b,0x00000078,0x00000007,0x0004003b,0x0000004b,
	0x0000007c,0x00000007,0x0004003b,0x0000004b,0x00000087,0x00000007,0x0004003b,0x00000056,
	0x00000099,0x00000007,0x00050041,0x00000013,0x00000014,0x00000011,0x00000012,0x0004003d,
	0x00000008,0x00000015,0x00000014,0x0003003e,0x0000000e,0x00000015,0x0004003d,0x00000008,
	0x00000017,0x0000000e,0x0003003e,0x00000016,0x00000017,0x000200f9,0x00000018,0x000200f8,
	0x00000018,0x000400f6,0x0000001a,0x0000001b,0x00000000,0x000200f9,0x0000001c,0x000200f8,
	0x0000001c,0x0004003d,0x00000008,0x0000001d,0x00000016,0x00050041,0x00000023,0x00000024,
	0x00000021,0x00000022,0x0004003d,0x0000001e,0x00000025,0x00000024,0x00050087,0x0000001e,
	0x00000027,0x00000025,0x00000026,0x0004007c,0x00000008,0x00000028,0x00000027,0x000500b0,
	0x00000029,0x0000002a,0x0000001d,0x00000028,0x000400fa,0x0000002a,0x00000019,0x0000001a,
	0x000200f8,0x00000019,0x0004003d,0x00000008,0x00000030,0x00000016,0x0004003d,0x00000008,
	0x00000035,0x00000016,0x00060041,0x00000036,0x00000037,0x00000034,0x00000022,0x00000035,
	0x0004003d,0x0000002b,0x00000038,0x00000037,0x00050041,0x00000039,0x0000003a,0x0000002f,
	0x00000030,0x0003003e,0x0000003a,0x00000038,0x0004003d,0x00000008,0x0000003c,0x00000016,
	0x00050080,0x00000008,0x0000003d,0x0000003c,0x0000003b,0x0003003e,0x00000016,0x0000003d,
	0x000200f9,0x0000001b,0x000200f8,0x0000001b,0x000200f9,0x00000018,0x000200f8,0x0000001a,
	0x00040039,0x00000002,0x0000003e,0x00000006,0x0004003d,0x00000008,0x00000040,0x0000000e,
	0x0003003e,0x0000003f,0x00000040,0x000200f9,0x00000041,0x000200f8,0x00000041,0x000400f6,
	0x00000043,0x00000044,0x00000000,0x000200f9,0x00000045,0x000200f8,0x00000045,0x0004003d,
	0x00000008,0x00000046,0x0000003f,0x00050041,0x00000023,0x00000047,0x00000021,0x00000022,
	0x0004003d,0x0000001e,0x00000048,0x00000047,0x0004007c,0x00000008,0x00000049,0x00000048,
	0x000500b0,0x00000029,0x0000004a,0x00000046,0x00000049,0x000400fa,0x0000004a,0x00000042,
	0x00000043,0x000200f8,0x00000042,0x0004003d,0x00000008,0x0000004d,0x0000003f,0x0004007c,
	0x0000001e,0x0000004e,0x0000004d,0x0004003d,0x00000008,0x0000004f,0x0000003f,0x0004007c,
	0x0000001e,0x00000050,0x0000004f,0x0004003d,0x00000008,0x00000051,0x0000003f,0x0004007c,
	0x0000001e,0x00000052,0x00000051,0x0004003d,0x00000008,0x00000053,0x0000003f,0x0004007c,
	0x0000001e,0x00000054,0x00000053,0x00070050,0x0000002b,0x00000055,0x0000004e,0x00000050,
	0x00000052,0x00000054,0x0003003e,0x0000004c,0x00000055,0x0004003d,0x00000008,0x00000058,
	0x0000003f,0x00050086,0x00000008,0x0000005a,0x00000058,0x00000059,0x0004003d,0x00000008,
	0x0000005b,0x0000003f,0x00050089,0x00000008,0x0000005c,0x0000005b,0x00000059,0x00060041,
	0x0000005d,0x0000005e,0x0000002f,0x0000005a,0x0000005c,0x0004003d,0x0000001e,0x0000005f,
	0x0000005e,0x0003003e,0x00000057,0x0000005f,0x0004003d,0x0000001e,0x00000061,0x00000057,
	0x0004003d,0x0000001e,0x00000062,0x00000057,0x0004003d,0x0000001e,0x00000063,0x00000057,
	0x0004003d,0x0000001e,0x00000064,0x00000057,0x00070050,0x0000002b,0x00000065,0x00000061,
	0x00000062,0x00000063,0x00000064,0x0003003e,0x00000060,0x00000065,0x0003003e,0x00000066,
	0x00000067,0x0003003e,0x00000068,0x0000006c,0x0003003e,0x0000006d,0x00000022,0x000200f9,
	0x0000006e,0x000200f8,0x0000006e,0x000400f6,0x00000070,0x00000071,0x00000000,0x000200f9,
	0x00000072,0x000200f8,0x00000072,0x0004003d,0x0000001e,0x00000073,0x0000006d,0x00050041,
	0x00000023,0x00000074,0x00000021,0x00000022,0x0004003d,0x0000001e,0x00000075,0x00000074,
	0x00050087,0x0000001e,0x00000076,0x00000075,0x00000026,0x000500b1,0x00000029,0x00000077,
	0x00000073,0x00000076,0x000400fa,0x00000077,0x0000006f,0x00000070,0x000200f8,0x0000006f,
	0x0004003d,0x0000001e,0x00000079,0x0000006d,0x00050041,0x00000039,0x0000007a,0x0000002f,
	0x00000079,0x0004003d,0x0000002b,0x0000007b,0x0000007a,0x0003003e,0x00000078,0x0000007b,
	0x0004003d,0x0000002b,0x0000007d,0x00000068,0x0004003d,0x0000002b,0x0000007e,0x0000004c,
	0x00050082,0x0000002b,0x0000007f,0x0000007d,0x0000007e,0x0003003e,0x0000007c,0x0000007f,
	0x0004003d,0x0000002b,0x00000080,0x0000007c,0x000500c3,0x0000002b,0x00000083,0x00000080,
	0x00000082,0x0003003e,0x0000007c,0x00000083,0x0004003d,0x0000002b,0x00000084,0x0000007c,
	0x000500c7,0x0000002b,0x00000086,0x00000084,0x00000085,0x0003003e,0x0000007c,0x00000086,
	0x0004003d,0x0000002b,0x00000088,0x00000078,0x0004003d,0x0000002b,0x00000089,0x00000060,
	0x00050082,0x0000002b,0x0000008a,0x00000088,0x00000089,0x0004003d,0x0000002b,0x0000008b,
	0x0000007c,0x00050082,0x0000002b,0x0000008c,0x0000008a,0x0000008b,0x0003003e,0x00000087,
	0x0000008c,0x0004003d,0x0000002b,0x0000008d,0x00000087,0x000500c3,0x0000002b,0x0000008e,
	0x0000008d,0x00000082,0x0003003e,0x00000087,0x0000008e,0x0004003d,0x0000002b,0x0000008f,
	0x00000087,0x000500c7,0x0000002b,0x00000090,0x0000008f,0x00000085,0x0003003e,0x00000087,
	0x00000090,0x0004003d,0x0000002b,0x00000091,0x00000087,0x0004003d,0x0000002b,0x00000092,
	0x00000066,0x00050080,0x0000002b,0x00000093,0x00000092,0x00000091,0x0003003e,0x00000066,
	0x00000093,0x0004003d,0x0000002b,0x00000095,0x00000068,0x00050080,0x0000002b,0x00000096,
	0x00000095,0x00000094,0x0003003e,0x00000068,0x00000096,0x0004003d,0x0000001e,0x00000097,
	0x0000006d,0x00050080,0x0000001e,0x00000098,0x00000097,0x00000069,0x0003003e,0x0000006d,
	0x00000098,0x000200f9,0x00000071,0x000200f8,0x00000071,0x000200f9,0x0000006e,0x000200f8,
	0x00000070,0x00050041,0x00000056,0x0000009a,0x00000066,0x00000012,0x0004003d,0x0000001e,
	0x0000009b,0x0000009a,0x00050041,0x00000056,0x0000009c,0x00000066,0x00000009,0x0004003d,
	0x0000001e,0x0000009d,0x0000009c,0x00050080,0x0000001e,0x0000009e,0x0000009b,0x0000009d,
	0x00050041,0x00000056,0x0000009f,0x00000066,0x0000000b,0x0004003d,0x0000001e,0x000000a0,
	0x0000009f,0x00050080,0x0000001e,0x000000a1,0x0000009e,0x000000a0,0x00050041,0x00000056,
	0x000000a3,0x00000066,0x000000a2,0x0004003d,0x0000001e,0x000000a4,0x000000a3,0x00050080,
	0x0000001e,0x000000a5,0x000000a1,0x000000a4,0x0003003e,0x00000099,0x000000a5,0x0004003d,
	0x0000001e,0x000000aa,0x00000099,0x0004003d,0x0000001e,0x000000ab,0x00000057,0x00060041,
	0x00000023,0x000000ac,0x000000a9,0x00000022,0x000000aa,0x0003003e,0x000000ac,0x000000ab,
	0x0004003d,0x00000008,0x000000ad,0x0000003f,0x00050080,0x00000008,0x000000ae,0x000000ad,
	0x0000003b,0x0003003e,0x0000003f,0x000000ae,0x000200f9,0x00000044,0x000200f8,0x00000044,
	0x000200f9,0x00000041,0x000200f8,0x00000043,0x000100fd,0x00010038,0x00050036,0x00000002,
	0x00000006,0x00000000,0x00000003,0x000200f8,0x00000007,0x000300e1,0x00000009,0x0000000a,
	0x000300e1,0x0000000b,0x0000000c,0x000300e1,0x00000009,0x0000000c,0x000400e0,0x0000000b,
	0x0000000b,0x0000000a,0x000100fd,0x00010038
};
