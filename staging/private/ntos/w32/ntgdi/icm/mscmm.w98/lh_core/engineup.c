/*
	File:		LHCalcEngineOnlyLookup.c

	Contains:	

	Written by:	U. J. Krabbenhoeft

	Version:

	Copyright:	� 1993-1997 by Heidelberger Druckmaschinen AG, all rights reserved.

*/

#undef LH_DATA_IN_TYPE
#undef LH_DATA_OUT_TYPE
#undef LH_LUT_DATA_TYPE
#undef LH_DATA_IN_COUNT
#undef LH_DATA_OUT_COUNT
#undef LH_DATA_SHR
#undef LH_DATA_SHR_CORR
#undef LH_LUT_DATA_SHR
#undef LH_BIT_BREIT_INTERNAL
#if LH_DATA_IN_SIZE_16
#define LH_DATA_IN_TYPE LH_UINT16
#else
#define LH_DATA_IN_TYPE LH_UINT8
#endif
#if LH_DATA_OUT_SIZE_16
#define LH_DATA_OUT_TYPE LH_UINT16
#else
#define LH_DATA_OUT_TYPE LH_UINT8
#endif
#if LH_LUT_DATA_SIZE_16
#define LH_BIT_BREIT_INTERNAL 	16
#define LH_LUT_DATA_TYPE LH_UINT16
#else
#define LH_BIT_BREIT_INTERNAL 	10
#define LH_LUT_DATA_TYPE LH_UINT8
#endif

#if LH_DATA_IN_COUNT_4
#define LH_DATA_IN_COUNT 		4
#else
#define LH_DATA_IN_COUNT 		3
#endif

#if LH_DATA_OUT_COUNT_4
#define LH_DATA_OUT_COUNT 		4
#else
#define LH_DATA_OUT_COUNT 		3
#endif

#define LH_BIT_MASKE_ADR (((1<<LH_BIT_BREIT_ADR)-1)<< (LH_BIT_BREIT_INTERNAL-LH_BIT_BREIT_ADR))
#define LH_BIT_BREIT_SELEKTOR (LH_BIT_BREIT_INTERNAL-LH_BIT_BREIT_ADR)
#define LH_BIT_MASKE_SELEKTOR ((1<<LH_BIT_BREIT_SELEKTOR)-1)

#define LH_ADR_BEREICH_SEL 		(1<<LH_BIT_BREIT_SELEKTOR)

#if LH_LUT_DATA_SIZE_16
#define LH_DATA_SHR               (16-LH_ADR_BREIT_AUS_LUT)  /* z.B. 16+11-10=17 */
#define LH_DATA_SHR_CORR 0		/* notwendig bei LH_DATA_SHR > 16 */
#define LH_LUT_DATA_SHR  16		/* Normierung bei Alutinterpolation */
#else
#define LH_DATA_SHR               (LH_ADR_BREIT_AUS_LUT-8)   /* z.B. 10-8=2 */
#define LH_LUT_DATA_SHR  8		/* Normierung bei Alutinterpolation */
#endif

#if LH_DATA_IN_COUNT_4
{

 	LH_UINT32 ein_regY;
	LH_UINT32 ein_regM;
	LH_UINT32 ein_regC;
	LH_UINT32 ein_regK;
	LH_DATA_IN_TYPE ein_cache[4];
	LH_LUT_DATA_TYPE * paNewVal0;

	LH_UINT8 	Mode;
	LH_UINT32 	PixelCount, LineCount, i, j;
	long inputOffset,outputOffset;
	LH_DATA_IN_TYPE * input0 = (LH_DATA_IN_TYPE *)calcParam->inputData[0];
	LH_DATA_IN_TYPE * input1 = (LH_DATA_IN_TYPE *)calcParam->inputData[1];
	LH_DATA_IN_TYPE * input2 = (LH_DATA_IN_TYPE *)calcParam->inputData[2];
	LH_DATA_IN_TYPE * input3 = (LH_DATA_IN_TYPE *)calcParam->inputData[3];
	LH_DATA_IN_TYPE * input4 = (LH_DATA_IN_TYPE *)calcParam->inputData[4];

	LH_DATA_OUT_TYPE * output0 = (LH_DATA_OUT_TYPE *)calcParam->outputData[0];
	LH_DATA_OUT_TYPE * output1 = (LH_DATA_OUT_TYPE *)calcParam->outputData[1];
	LH_DATA_OUT_TYPE * output2 = (LH_DATA_OUT_TYPE *)calcParam->outputData[2];
	LH_DATA_OUT_TYPE * output3 = (LH_DATA_OUT_TYPE *)calcParam->outputData[3];
	LH_DATA_OUT_TYPE * output4 = (LH_DATA_OUT_TYPE *)calcParam->outputData[4];

	LH_UINT16 * My_InputLut = (LH_UINT16 *)lutParam->inputLut;
	LH_LUT_DATA_TYPE * My_OutputLut = (LH_LUT_DATA_TYPE *)lutParam->outputLut;
	LH_LUT_DATA_TYPE * My_ColorLut = (LH_LUT_DATA_TYPE *)lutParam->colorLut;

	LH_DATA_OUT_TYPE Mask = (LH_DATA_OUT_TYPE)-1;

#ifdef DEBUG_OUTPUT
	CMError err = noErr;
#endif
	LH_START_PROC(LH_CALC_PROC_NAME)

	#if LH_DATA_IN_SIZE_16
	inputOffset = (long)calcParam->cmInputPixelOffset / 2;
	#else
	inputOffset = (long)calcParam->cmInputPixelOffset;
	#endif
	#if LH_DATA_OUT_SIZE_16
	outputOffset = (long)calcParam->cmOutputPixelOffset / 2;
	#else
	outputOffset = (long)calcParam->cmOutputPixelOffset;
	#endif

	if (calcParam->clearMask)
		Mask = 0;
	Mode = LH_CALC_ENGINE_UNDEF_MODE;


	if ((calcParam->cmInputPixelOffset * calcParam->cmPixelPerLine == calcParam->cmInputBytesPerLine) && (calcParam->cmOutputPixelOffset * calcParam->cmPixelPerLine == calcParam->cmOutputBytesPerLine))
	{
		PixelCount = calcParam->cmPixelPerLine * calcParam->cmLineCount;
		LineCount = 1;
	}
	else
	{
		PixelCount = calcParam->cmPixelPerLine;
		LineCount = calcParam->cmLineCount;
	}
	if (calcParam->copyAlpha )
	{
			Mode = LH_CALC_ENGINE_U_TO_U;
	}
	else
	{
		if (calcParam->clearMask)
			Mode = LH_CALC_ENGINE_P_TO_U;
		else
			Mode = LH_CALC_ENGINE_P_TO_P;
	}

 	j = 0;
	while (LineCount)
	{
		i = PixelCount;
		while (i)
		{
			#if LH_LUT_DATA_SIZE_16
			#if LH_DATA_OUT_SIZE_16
    	   		register LH_UINT32 ko;
	       		register LH_UINT32 aVal;
    	   	#endif
    	   	#endif
			#if LH_DATA_IN_SIZE_16
			ein_regC = My_InputLut[((ein_cache[0]=*input0)>>( 16-LH_ADR_BREIT_EIN_LUT )) + ( 0 << LH_ADR_BREIT_EIN_LUT )];
			ein_regC += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regM = My_InputLut[((ein_cache[1]=*input1)>>( 16-LH_ADR_BREIT_EIN_LUT )) + ( 1 << LH_ADR_BREIT_EIN_LUT )];
			ein_regM += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regY = My_InputLut[((ein_cache[2]=*input2)>>( 16-LH_ADR_BREIT_EIN_LUT )) + ( 2 << LH_ADR_BREIT_EIN_LUT )];
			ein_regY += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regK = My_InputLut[((ein_cache[3]=*input3)>>( 16-LH_ADR_BREIT_EIN_LUT )) + ( 3 << LH_ADR_BREIT_EIN_LUT )];
			ein_regK += 1<<LH_BIT_BREIT_SELEKTOR-1;
			#else
			ein_regC = My_InputLut[(ein_cache[0]=*input0) + ( 0 << LH_ADR_BREIT_EIN_LUT )];
			ein_regC += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regM = My_InputLut[(ein_cache[1]=*input1) + ( 1 << LH_ADR_BREIT_EIN_LUT )];
			ein_regM += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regY = My_InputLut[(ein_cache[2]=*input2) + ( 2 << LH_ADR_BREIT_EIN_LUT )];
			ein_regY += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regK = My_InputLut[(ein_cache[3]=*input3) + ( 3 << LH_ADR_BREIT_EIN_LUT )];
			ein_regK += 1<<LH_BIT_BREIT_SELEKTOR-1;
			#endif
			paNewVal0 = (LH_LUT_DATA_TYPE *)My_ColorLut + 
						((((((((ein_regC & LH_BIT_MASKE_ADR) << LH_BIT_BREIT_ADR) +
							   (ein_regM & LH_BIT_MASKE_ADR)) << LH_BIT_BREIT_ADR) +
							   (ein_regY & LH_BIT_MASKE_ADR))>> (LH_BIT_BREIT_SELEKTOR-LH_BIT_BREIT_ADR)) + 
						       (ein_regK >> LH_BIT_BREIT_SELEKTOR))*LH_DATA_OUT_COUNT);
		       				#if LH_LUT_DATA_SIZE_16
							#if LH_DATA_OUT_SIZE_16
			   		
							aVal =					 (	paNewVal0[0]);
							aVal = aVal + ( aVal >> ( LH_LUT_DATA_SHR ));
							aVal = aVal - ( aVal >> ( LH_ADR_BREIT_AUS_LUT ));
							ko = (aVal>>LH_DATA_SHR_CORR) & ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR))-1 );
							aVal = (aVal >> ( LH_DATA_SHR )) + ( 0 << LH_ADR_BREIT_AUS_LUT );
					       	*output0 = (LH_DATA_OUT_TYPE)((My_OutputLut[aVal] * ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR )) - ko ) + My_OutputLut[aVal +1] * ko)>>(LH_DATA_SHR - LH_DATA_SHR_CORR) );
	
							aVal =					 (	paNewVal0[1]);
							aVal = aVal + ( aVal >> ( LH_LUT_DATA_SHR ));
							aVal = aVal - ( aVal >> ( LH_ADR_BREIT_AUS_LUT ));
							ko = (aVal>>LH_DATA_SHR_CORR) & ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR))-1 );
							aVal = (aVal >> ( LH_DATA_SHR )) + ( 1 << LH_ADR_BREIT_AUS_LUT );
					       	*output1 = (LH_DATA_OUT_TYPE)((My_OutputLut[aVal] * ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR )) - ko ) + My_OutputLut[aVal +1] * ko)>>(LH_DATA_SHR - LH_DATA_SHR_CORR) );
	
							aVal =					 (	paNewVal0[2]);
							aVal = aVal + ( aVal >> ( LH_LUT_DATA_SHR ));
							aVal = aVal - ( aVal >> ( LH_ADR_BREIT_AUS_LUT ));
							ko = (aVal>>LH_DATA_SHR_CORR) & ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR))-1 );
							aVal = (aVal >> ( LH_DATA_SHR )) + ( 2 << LH_ADR_BREIT_AUS_LUT );
					       	*output2 = (LH_DATA_OUT_TYPE)((My_OutputLut[aVal] * ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR )) - ko ) + My_OutputLut[aVal +1] * ko)>>(LH_DATA_SHR - LH_DATA_SHR_CORR) );
	
							#if LH_DATA_OUT_COUNT_4
							aVal =					 (	paNewVal0[3]);
							aVal = aVal + ( aVal >> ( LH_LUT_DATA_SHR ));
							aVal = aVal - ( aVal >> ( LH_ADR_BREIT_AUS_LUT ));
							ko = (aVal>>LH_DATA_SHR_CORR) & ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR))-1 );
							aVal = (aVal >> ( LH_DATA_SHR )) + ( 3 << LH_ADR_BREIT_AUS_LUT );
					       	*output3 = (LH_DATA_OUT_TYPE)((My_OutputLut[aVal] * ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR )) - ko ) + My_OutputLut[aVal +1] * ko)>>(LH_DATA_SHR - LH_DATA_SHR_CORR) );
							#endif
							
							#else
							*output0 = My_OutputLut[((	paNewVal0[0])>>LH_DATA_SHR) + (0<<LH_ADR_BREIT_AUS_LUT)]>>8;
							*output1 = My_OutputLut[((	paNewVal0[1])>>LH_DATA_SHR) + (1<<LH_ADR_BREIT_AUS_LUT)]>>8;
							*output2 = My_OutputLut[((	paNewVal0[2])>>LH_DATA_SHR) + (2<<LH_ADR_BREIT_AUS_LUT)]>>8;
							#if LH_DATA_OUT_COUNT_4
							*output3 = My_OutputLut[((	paNewVal0[3])>>LH_DATA_SHR) + (3<<LH_ADR_BREIT_AUS_LUT)]>>8;
							#endif
							#endif
							
							#else
							*output0 = My_OutputLut[((	paNewVal0[0])<<LH_DATA_SHR) + (0<<LH_ADR_BREIT_AUS_LUT)];
							*output1 = My_OutputLut[((	paNewVal0[1])<<LH_DATA_SHR) + (1<<LH_ADR_BREIT_AUS_LUT)];
							*output2 = My_OutputLut[((	paNewVal0[2])<<LH_DATA_SHR) + (2<<LH_ADR_BREIT_AUS_LUT)];
							#if LH_DATA_OUT_COUNT_4
							*output3 = My_OutputLut[((	paNewVal0[3])<<LH_DATA_SHR) + (3<<LH_ADR_BREIT_AUS_LUT)];
							#endif
							#endif

			#if LH_DATA_OUT_SIZE_16 && ! LH_LUT_DATA_SIZE_16
			*output0 |= (*output0 << 8);
			*output1 |= (*output1 << 8);
			*output2 |= (*output2 << 8);
			#if LH_DATA_OUT_COUNT_4
			*output3 |= (*output3 << 8);
			#endif
			#endif
			
			if (Mode == LH_CALC_ENGINE_P_TO_P)
			{
				while (--i)
				{
					input0 += inputOffset;
					input1 += inputOffset;
					input2 += inputOffset;
					input3 += inputOffset;
					output0 += outputOffset;
					output1 += outputOffset;
					output2 += outputOffset;
					#if LH_DATA_OUT_COUNT_4
					output3 += outputOffset;
					#endif

					if ((*input0 ^ ein_cache[0]) || (*input1 ^ ein_cache[1]) || (*input2 ^ ein_cache[2]) || (*input3 ^ ein_cache[3]))
					{
						break;
					}
					*output0 = output0[-outputOffset];
					*output1 = output1[-outputOffset];
					*output2 = output2[-outputOffset];
					#if LH_DATA_OUT_COUNT_4
					*output3 = output3[-outputOffset];
					#endif
				}
			}
			else if (Mode == LH_CALC_ENGINE_P_TO_U)
			{
				#if LH_DATA_OUT_COUNT_4
				*output4 &= Mask;
				#else
				*output3 &= Mask;
				#endif
				while (--i)
				{
					input0 += inputOffset;
					input1 += inputOffset;
					input2 += inputOffset;
					input3 += inputOffset;
					output0 += outputOffset;
					output1 += outputOffset;
					output2 += outputOffset;
					output3 += outputOffset;
					#if LH_DATA_OUT_COUNT_4
					output4 += outputOffset;
					#endif
					if ((*input0 ^ ein_cache[0]) || (*input1 ^ ein_cache[1]) || (*input2 ^ ein_cache[2]) || (*input3 ^ ein_cache[3]))
					{
						break;
					}
					*output0 = output0[-outputOffset];
					*output1 = output1[-outputOffset];
					*output2 = output2[-outputOffset];
					#if LH_DATA_OUT_COUNT_4
					*output3 = output3[-outputOffset];
					*output4 &= Mask;
					#else
					*output3 &= Mask;
					#endif
				}
			}
			else
			{
				#if LH_DATA_OUT_COUNT_4
				*output4 = (LH_DATA_OUT_TYPE)*input4;
				#else
				*output3 = (LH_DATA_OUT_TYPE)*input4;
				#endif
				while (--i)
				{								/*U_TO_U*/
					input0 += inputOffset;
					input1 += inputOffset;
					input2 += inputOffset;
					input3 += inputOffset;
					input4 += inputOffset;

					output0 += outputOffset;
					output1 += outputOffset;
					output2 += outputOffset;
					output3 += outputOffset;
					#if LH_DATA_OUT_COUNT_4
					output4 += outputOffset;
					#endif
					if ((*input0 ^ ein_cache[0]) || (*input1 ^ ein_cache[1]) || (*input2 ^ ein_cache[2]) || (*input3 ^ ein_cache[3]))
					{
						break;
					}
					*output0 = output0[-outputOffset];
					*output1 = output1[-outputOffset];
					*output2 = output2[-outputOffset];
					#if LH_DATA_OUT_COUNT_4
					*output3 = output3[-outputOffset];
					*output4 = (LH_DATA_OUT_TYPE)*input4;
					#else
					*output3 = (LH_DATA_OUT_TYPE)*input4;
					#endif
				}
			}
		}
		if (--LineCount)
		{
			j++;
			input0 = (LH_DATA_IN_TYPE *)((LH_UINT8 *)calcParam->inputData[0] + j * calcParam->cmInputBytesPerLine);
			input1 = (LH_DATA_IN_TYPE *)((LH_UINT8 *)calcParam->inputData[1] + j * calcParam->cmInputBytesPerLine);
			input2 = (LH_DATA_IN_TYPE *)((LH_UINT8 *)calcParam->inputData[2] + j * calcParam->cmInputBytesPerLine);
			input3 = (LH_DATA_IN_TYPE *)((LH_UINT8 *)calcParam->inputData[3] + j * calcParam->cmInputBytesPerLine);
			input4 = (LH_DATA_IN_TYPE *)((LH_UINT8 *)calcParam->inputData[4] + j * calcParam->cmInputBytesPerLine);

			output0 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[0] + j * calcParam->cmOutputBytesPerLine);
			output1 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[1] + j * calcParam->cmOutputBytesPerLine);
			output2 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[2] + j * calcParam->cmOutputBytesPerLine);
			output3 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[3] + j * calcParam->cmOutputBytesPerLine);
			#if LH_DATA_OUT_COUNT_4
			output4 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[4] + j * calcParam->cmOutputBytesPerLine);
			#endif
		}
	}
	LH_END_PROC(LH_CALC_PROC_NAME)
	return 0;
}
#else
{

 	LH_UINT32 ein_regb;
	LH_UINT32 ein_regg;
	LH_UINT32 ein_regr;
	LH_DATA_IN_TYPE ein_cache[3];
	LH_LUT_DATA_TYPE * paNewVal0;

	LH_UINT8 	Mode;
	LH_UINT32 	PixelCount, LineCount, i, j;
	long inputOffset,outputOffset;
	LH_DATA_IN_TYPE * input0 = (LH_DATA_IN_TYPE *)calcParam->inputData[0];
	LH_DATA_IN_TYPE * input1 = (LH_DATA_IN_TYPE *)calcParam->inputData[1];
	LH_DATA_IN_TYPE * input2 = (LH_DATA_IN_TYPE *)calcParam->inputData[2];
	LH_DATA_IN_TYPE * input3 = (LH_DATA_IN_TYPE *)calcParam->inputData[3];

	LH_DATA_OUT_TYPE * output0 = (LH_DATA_OUT_TYPE *)calcParam->outputData[0];
	LH_DATA_OUT_TYPE * output1 = (LH_DATA_OUT_TYPE *)calcParam->outputData[1];
	LH_DATA_OUT_TYPE * output2 = (LH_DATA_OUT_TYPE *)calcParam->outputData[2];
	LH_DATA_OUT_TYPE * output3 = (LH_DATA_OUT_TYPE *)calcParam->outputData[3];
	LH_DATA_OUT_TYPE * output4 = (LH_DATA_OUT_TYPE *)calcParam->outputData[4];

	LH_UINT16 * My_InputLut = (LH_UINT16 *)lutParam->inputLut;
	LH_LUT_DATA_TYPE * My_OutputLut = (LH_LUT_DATA_TYPE *)lutParam->outputLut;
	LH_LUT_DATA_TYPE * My_ColorLut = (LH_LUT_DATA_TYPE *)lutParam->colorLut;

	LH_DATA_OUT_TYPE Mask = (LH_DATA_OUT_TYPE)-1;

#ifdef DEBUG_OUTPUT
	CMError err = noErr;
#endif
	LH_START_PROC(LH_CALC_PROC_NAME)

	#if LH_DATA_IN_SIZE_16
	inputOffset = (long)calcParam->cmInputPixelOffset / 2;
	#else
	inputOffset = (long)calcParam->cmInputPixelOffset;
	#endif
	#if LH_DATA_OUT_SIZE_16
	outputOffset = (long)calcParam->cmOutputPixelOffset / 2;
	#else
	outputOffset = (long)calcParam->cmOutputPixelOffset;
	#endif

	if (calcParam->clearMask)
		Mask = 0;
	Mode = LH_CALC_ENGINE_UNDEF_MODE;


	if ((calcParam->cmInputPixelOffset * calcParam->cmPixelPerLine == calcParam->cmInputBytesPerLine) && (calcParam->cmOutputPixelOffset * calcParam->cmPixelPerLine == calcParam->cmOutputBytesPerLine))
	{
		PixelCount = calcParam->cmPixelPerLine * calcParam->cmLineCount;
		LineCount = 1;
	}
	else
	{
		PixelCount = calcParam->cmPixelPerLine;
		LineCount = calcParam->cmLineCount;
	}
	if (calcParam->copyAlpha )
	{
			Mode = LH_CALC_ENGINE_U_TO_U;
	}
	else
	{
		if (calcParam->clearMask)
			Mode = LH_CALC_ENGINE_P_TO_U;
		else
			Mode = LH_CALC_ENGINE_P_TO_P;
	}
 	j = 0;
	while (LineCount)
	{
		i = PixelCount;
		while (i)
		{
			#if LH_LUT_DATA_SIZE_16
			#if LH_DATA_OUT_SIZE_16
    	   		register LH_UINT32 ko;
	       		register LH_UINT32 aVal;
    	   	#endif
    	   	#endif
			#if LH_DATA_IN_SIZE_16
			ein_regr = My_InputLut[((ein_cache[0]=*input0)>>( 16-LH_ADR_BREIT_EIN_LUT )) + ( 0 << LH_ADR_BREIT_EIN_LUT )];
			ein_regr += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regg = My_InputLut[((ein_cache[1]=*input1)>>( 16-LH_ADR_BREIT_EIN_LUT )) + ( 1 << LH_ADR_BREIT_EIN_LUT )];
			ein_regg += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regb = My_InputLut[((ein_cache[2]=*input2)>>( 16-LH_ADR_BREIT_EIN_LUT )) + ( 2 << LH_ADR_BREIT_EIN_LUT )];
			ein_regb += 1<<LH_BIT_BREIT_SELEKTOR-1;
			#else
			ein_regr = My_InputLut[(ein_cache[0]=*input0) + ( 0 << LH_ADR_BREIT_EIN_LUT )];
			ein_regr += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regg = My_InputLut[(ein_cache[1]=*input1) + ( 1 << LH_ADR_BREIT_EIN_LUT )];
			ein_regg += 1<<LH_BIT_BREIT_SELEKTOR-1;
			ein_regb = My_InputLut[(ein_cache[2]=*input2) + ( 2 << LH_ADR_BREIT_EIN_LUT )];
			ein_regb += 1<<LH_BIT_BREIT_SELEKTOR-1;
			#endif
			paNewVal0 = (LH_LUT_DATA_TYPE *)My_ColorLut + 
						((((((ein_regr & LH_BIT_MASKE_ADR) << LH_BIT_BREIT_ADR) +
							 (ein_regg & LH_BIT_MASKE_ADR))>> (LH_BIT_BREIT_SELEKTOR-LH_BIT_BREIT_ADR)) + 
						     (ein_regb >> LH_BIT_BREIT_SELEKTOR))*LH_DATA_OUT_COUNT);
						#if LH_LUT_DATA_SIZE_16
						#if LH_DATA_OUT_SIZE_16
		   		
						aVal =					 ( paNewVal0[0]);
						aVal = aVal + ( aVal >> ( LH_LUT_DATA_SHR ));
						aVal = aVal - ( aVal >> ( LH_ADR_BREIT_AUS_LUT ));
						ko = (aVal>>LH_DATA_SHR_CORR) & ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR))-1 );
						aVal = (aVal >> ( LH_DATA_SHR )) + ( 0 << LH_ADR_BREIT_AUS_LUT );
				       	*output0 = (LH_DATA_OUT_TYPE)((My_OutputLut[aVal] * ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR )) - ko ) + My_OutputLut[aVal +1] * ko)>>(LH_DATA_SHR - LH_DATA_SHR_CORR) );

						paNewVal0++;
						aVal =					 ( paNewVal0[0]);
						aVal = aVal + ( aVal >> ( LH_LUT_DATA_SHR ));
						aVal = aVal - ( aVal >> ( LH_ADR_BREIT_AUS_LUT ));
						ko = (aVal>>LH_DATA_SHR_CORR) & ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR))-1 );
						aVal = (aVal >> ( LH_DATA_SHR )) + ( 1 << LH_ADR_BREIT_AUS_LUT );
				       	*output1 = (LH_DATA_OUT_TYPE)((My_OutputLut[aVal] * ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR )) - ko ) + My_OutputLut[aVal +1] * ko)>>(LH_DATA_SHR - LH_DATA_SHR_CORR) );

						paNewVal0++;
						aVal =					 ( paNewVal0[0]);
						aVal = aVal + ( aVal >> ( LH_LUT_DATA_SHR ));
						aVal = aVal - ( aVal >> ( LH_ADR_BREIT_AUS_LUT ));
						ko = (aVal>>LH_DATA_SHR_CORR) & ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR))-1 );
						aVal = (aVal >> ( LH_DATA_SHR )) + ( 2 << LH_ADR_BREIT_AUS_LUT );
				       	*output2 = (LH_DATA_OUT_TYPE)((My_OutputLut[aVal] * ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR )) - ko ) + My_OutputLut[aVal +1] * ko)>>(LH_DATA_SHR - LH_DATA_SHR_CORR) );

						#if LH_DATA_OUT_COUNT_4
						paNewVal0++;
						aVal =					 ( paNewVal0[0]);
						aVal = aVal + ( aVal >> ( LH_LUT_DATA_SHR ));
						aVal = aVal - ( aVal >> ( LH_ADR_BREIT_AUS_LUT ));
						ko = (aVal>>LH_DATA_SHR_CORR) & ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR))-1 );
						aVal = (aVal >> ( LH_DATA_SHR )) + ( 3 << LH_ADR_BREIT_AUS_LUT );
				       	*output3 = (LH_DATA_OUT_TYPE)((My_OutputLut[aVal] * ( (1<<( LH_DATA_SHR - LH_DATA_SHR_CORR )) - ko ) + My_OutputLut[aVal +1] * ko)>>(LH_DATA_SHR - LH_DATA_SHR_CORR) );
						#endif
						
						#else
						*output0 = My_OutputLut[(( paNewVal0[0])>>LH_DATA_SHR) + (0<<LH_ADR_BREIT_AUS_LUT)]>>8;
						paNewVal0++;
						*output1 = My_OutputLut[(( paNewVal0[0])>>LH_DATA_SHR) + (1<<LH_ADR_BREIT_AUS_LUT)]>>8;
						paNewVal0++;
						*output2 = My_OutputLut[(( paNewVal0[0])>>LH_DATA_SHR) + (2<<LH_ADR_BREIT_AUS_LUT)]>>8;
						#if LH_DATA_OUT_COUNT_4
						paNewVal0++;
						*output3 = My_OutputLut[(( paNewVal0[0])>>LH_DATA_SHR) + (3<<LH_ADR_BREIT_AUS_LUT)]>>8;
						#endif
						#endif
						
						#else
						*output0 = My_OutputLut[(( paNewVal0[0])<<LH_DATA_SHR) + (0<<LH_ADR_BREIT_AUS_LUT)];
						paNewVal0++;
						*output1 = My_OutputLut[(( paNewVal0[0])<<LH_DATA_SHR) + (1<<LH_ADR_BREIT_AUS_LUT)];
						paNewVal0++;
						*output2 = My_OutputLut[(( paNewVal0[0])<<LH_DATA_SHR) + (2<<LH_ADR_BREIT_AUS_LUT)];
						#if LH_DATA_OUT_COUNT_4
						paNewVal0++;
						*output3 = My_OutputLut[(( paNewVal0[0])<<LH_DATA_SHR) + (3<<LH_ADR_BREIT_AUS_LUT)];
						#endif
						#endif

			#if LH_DATA_OUT_SIZE_16 && ! LH_LUT_DATA_SIZE_16
			*output0 |= (*output0 << 8);
			*output1 |= (*output1 << 8);
			*output2 |= (*output2 << 8);
			#if LH_DATA_OUT_COUNT_4
			*output3 |= (*output3 << 8);
			#endif
			#endif
			
			if (Mode == LH_CALC_ENGINE_P_TO_P)
			{
				while (--i)
				{
					input0 += inputOffset;
					input1 += inputOffset;
					input2 += inputOffset;
					output0 += outputOffset;
					output1 += outputOffset;
					output2 += outputOffset;
					#if LH_DATA_OUT_COUNT_4
					output3 += outputOffset;
					#endif

					if ((*input0 ^ ein_cache[0]) || (*input1 ^ ein_cache[1]) || (*input2 ^ ein_cache[2]))
					{
						break;
					}
					*output0 = output0[-outputOffset];
					*output1 = output1[-outputOffset];
					*output2 = output2[-outputOffset];
					#if LH_DATA_OUT_COUNT_4
					*output3 = output3[-outputOffset];
					#endif
				}
			}
			else if (Mode == LH_CALC_ENGINE_P_TO_U)
			{
				#if LH_DATA_OUT_COUNT_4
				*output4 &= Mask;
				#else
				*output3 &= Mask;
				#endif
				while (--i)
				{
					input0 += inputOffset;
					input1 += inputOffset;
					input2 += inputOffset;
					output0 += outputOffset;
					output1 += outputOffset;
					output2 += outputOffset;
					output3 += outputOffset;
					#if LH_DATA_OUT_COUNT_4
					output4 += outputOffset;
					#endif
					if ((*input0 ^ ein_cache[0]) || (*input1 ^ ein_cache[1]) || (*input2 ^ ein_cache[2]))
					{
						break;
					}
					*output0 = output0[-outputOffset];
					*output1 = output1[-outputOffset];
					*output2 = output2[-outputOffset];
					#if LH_DATA_OUT_COUNT_4
					*output3 = output3[-outputOffset];
					*output4 &= Mask;
					#else
					*output3 &= Mask;
					#endif
				}
			}
			else
			{
				#if LH_DATA_OUT_COUNT_4
				*output4 = (LH_DATA_OUT_TYPE)*input3;
				#else
				*output3 = (LH_DATA_OUT_TYPE)*input3;
				#endif
				while (--i)
				{								/*U_TO_U*/
					input0 += inputOffset;
					input1 += inputOffset;
					input2 += inputOffset;
					input3 += inputOffset;

					output0 += outputOffset;
					output1 += outputOffset;
					output2 += outputOffset;
					output3 += outputOffset;
					#if LH_DATA_OUT_COUNT_4
					output4 += outputOffset;
					#endif
					if ((*input0 ^ ein_cache[0]) || (*input1 ^ ein_cache[1]) || (*input2 ^ ein_cache[2]))
					{
						break;
					}
					*output0 = output0[-outputOffset];
					*output1 = output1[-outputOffset];
					*output2 = output2[-outputOffset];
					#if LH_DATA_OUT_COUNT_4
					*output3 = output3[-outputOffset];
					*output4 = (LH_DATA_OUT_TYPE)*input3;
					#else
					*output3 = (LH_DATA_OUT_TYPE)*input3;
					#endif
				}
			}
		}
		if (--LineCount)
		{
			j++;
			input0 = (LH_DATA_IN_TYPE *)((LH_UINT8 *)calcParam->inputData[0] + j * calcParam->cmInputBytesPerLine);
			input1 = (LH_DATA_IN_TYPE *)((LH_UINT8 *)calcParam->inputData[1] + j * calcParam->cmInputBytesPerLine);
			input2 = (LH_DATA_IN_TYPE *)((LH_UINT8 *)calcParam->inputData[2] + j * calcParam->cmInputBytesPerLine);
			input3 = (LH_DATA_IN_TYPE *)((LH_UINT8 *)calcParam->inputData[3] + j * calcParam->cmInputBytesPerLine);

			output0 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[0] + j * calcParam->cmOutputBytesPerLine);
			output1 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[1] + j * calcParam->cmOutputBytesPerLine);
			output2 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[2] + j * calcParam->cmOutputBytesPerLine);
			output3 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[3] + j * calcParam->cmOutputBytesPerLine);
			#if LH_DATA_OUT_COUNT_4
			output4 = (LH_DATA_OUT_TYPE *)((LH_UINT8 *)calcParam->outputData[4] + j * calcParam->cmOutputBytesPerLine);
			#endif
		}
	}
	LH_END_PROC(LH_CALC_PROC_NAME)
	return 0;
}
#endif
#undef LH_CALC_PROC_NAME
