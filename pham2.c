/*Name: Yen Pham
Assignment 2
CS 5390
This program convert a color ppm image into 3 types of "grayscale" image: normal grayscale, desaturate and decompose

to compile: go to Putty using cse machine: cse01.cse.unt.edu
gcc pham2.c -lm

to run: ./a.out

*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h> 

typedef struct RGB
{
	unsigned char R,G,B;
}RGBpixel;

typedef struct Image
{
	int magic_identifier;
	int width, height;
	int max_value;
	RGBpixel *data;
}PPMimage;

int getmin(int R, int G, int B) // get min
{
	int min = R;
	if (G < min)
	{
		min = G;
	}

	if (B < min)
	{
		min = B;
	}
	return min;
}

int getmax(int R, int G, int B) // get max
{
	int max = R;
	if (G > max)
	{
		max = G;
	}

	if (B > max)
	{
		max = B;
	}
	return max;

}

void Readppm(char *filename, PPMimage *image) //open and read ppm file
{
	FILE *fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		printf("Error opening file. No file existed with that name\n");
		exit(0);
	}

	//read header
	char c = fgetc(fp);
	if (c != 'P')
	{
		printf("%s is not a valid ppm file\n", filename);
		exit (0);
	}
	c = fgetc(fp);
	if (c == '3') // get maic identifier
	{
		(*image).magic_identifier = 3;
	} 
	else if (c == '6')
	{
		(*image).magic_identifier = 6;
	}
	else
	{
		printf("%s is not a valid ppm file\n", filename);
		exit (0);
	}

	c = fgetc(fp);
	//if the next character is comment or space, "eat" that character and continue
	if ((c == '#') || isspace(c) )
	{
		while ((c == '#') || isspace(c) )
		{
			if (c == '#')
			{
				while (c != '\n')
				{
					c = fgetc(fp);
				}
			}
			else
			{
				while (isspace(c))
				{
					c = fgetc(fp);
				}
			}
		}
	}
	else
	{
		printf("%s is not a valid ppm file\n", filename);
		exit (0);
	}

	//read image width
	int count = 1;
	char digits[17];
	if(isdigit(c))
	{
		digits[0] = c;
		while(isdigit(c) && (count < 16) )
		{
			c = fgetc(fp);
			digits[count] = c;
			count++;
		} 
		digits[count] = '\0';
		(*image).width = atoi(digits);
	}
	else
	{
		printf("%s is not a valid ppm file\n", filename);
		exit (0);
	}

	//check if there is any comment or space after the space. If yes, eat the space and/or comment
	if (isspace(c))
	{
		while ((c == '#') || isspace(c) )
		{
			if (c == '#')
			{
				while (c != '\n')
				{
					c = fgetc(fp);
				}
			}
			else
			{
				while (isspace(c))
				{
					c = fgetc(fp);
				}
			}
		}
	}
	else
	{
		printf("%s is not a valid ppm file\n", filename);
		exit (0);
	}

	//read image height
	memset(digits, 0, 17);
	count = 1;
	if (isdigit(c))
	{
		digits[0] = c;
		while(isdigit(c) && (count < 16) )
		{
			c = fgetc(fp);
			digits[count] = c;
			count++;
		} 
		digits[count] = '\0';
		(*image).height = atoi(digits);
	}
	else
	{
		printf("%s is not a valid ppm file\n", filename);
		exit (0);
	}

	//check if there is any comment or space after the space. If yes, eat the space and/or comment
	if (isspace(c))
	{
		while ((c == '#') || isspace(c) )
		{
			if (c == '#')
			{
				while (c != '\n')
				{
					c = fgetc(fp);
				}
			}
			else
			{
				while (isspace(c))
				{
					c = fgetc(fp);
				}
			}
		}
	}
	else
	{
		printf("%s is not a valid ppm file\n", filename);
		exit (0);
	}

	//read image max value
	memset(digits, 0, 17);
	count = 1;
	if (isdigit(c))
	{
		digits[0] = c;
		while(isdigit(c) && (count < 16) )
		{
			c = fgetc(fp);
			digits[count] = c;
			count++;
		} 
		digits[count] = '\0';
		(*image).max_value = atoi(digits);
	}
	else
	{
		printf("%s is not a valid ppm file\n", filename);
		exit (0);
	}

	//printf("%d %d %d %d\n", (*image).magic_identifier, (*image).width, (*image).height, (*image).max_value);
	
	//check if there is any comment or space after the space. If yes, eat the space and/or comment
	if (!isspace(c))
	{
		printf("%s is not a valid ppm file\n", filename);
		exit (0);
	}
	
	(*image).data = malloc(sizeof(RGBpixel) * (*image).width * (*image).height);
	int temp;
	count = 0;
	
	//read image data
	if ((*image).magic_identifier == 3)
	{
		for (int i = 0; i < (*image).width * (*image).height; i++)
		{
			fscanf(fp, "%d", &temp);
			(*image).data[i].R = temp;
			fscanf(fp, "%d", &temp);
			(*image).data[i].G = temp;
			fscanf(fp, "%d", &temp);
			(*image).data[i].B = temp;
		}
	}

	if ((*image).magic_identifier == 6)
	{
		fread((*image).data, sizeof(RGBpixel), (*image).width * (*image).height, fp);
	}

	/* test
	for (int i = 0; i < (*image).width * (*image).height; i++)
	{
		printf("%d %d %d\n", (*image).data[i].R, (*image).data[i].G, (*image).data[i].B);
	}
	*/
}

void Createppm(PPMimage original_img, PPMimage *grayscale_img, PPMimage *desaturate_img, PPMimage *decompose_img)
{
	//Create grayscale image
	(*grayscale_img).magic_identifier = original_img.magic_identifier;
	(*grayscale_img).width = original_img.width;
	(*grayscale_img).height = original_img.height;
	(*grayscale_img).max_value = original_img.max_value;
	(*grayscale_img).data = malloc(sizeof(RGBpixel) * original_img.width * original_img.height);

	for (int i = 0; i < (*grayscale_img).width*(*grayscale_img).height; i++)
	{	
		(*grayscale_img).data[i].R = round(0.3*(double)original_img.data[i].R + 0.59*(double)original_img.data[i].G + 0.11*(double)original_img.data[i].B);
		(*grayscale_img).data[i].G = (*grayscale_img).data[i].R;
		(*grayscale_img).data[i].B = (*grayscale_img).data[i].R;
	}

	//printf("%d %d %d\n", original_img.data[0].R, original_img.data[0].G, original_img.data[0].B);
	//printf("%d %d %d", (*grayscale_img).data[0].R, (*grayscale_img).data[0].G, (*grayscale_img).data[0].B);

	//Create desaturate image
	(*desaturate_img).magic_identifier = original_img.magic_identifier;
	(*desaturate_img).width = original_img.width;
	(*desaturate_img).height = original_img.height;
	(*desaturate_img).max_value = original_img.max_value;
	(*desaturate_img).data = malloc(sizeof(RGBpixel) * original_img.width * original_img.height);

	for (int i = 0; i < (*desaturate_img).width*(*desaturate_img).height; i++)
	{
		(*desaturate_img).data[i].R = round( (double)(getmax(original_img.data[i].R, original_img.data[i].G, original_img.data[i].B) + getmin(original_img.data[i].R, original_img.data[i].G, original_img.data[i].B)) / 2.0 ) ;
		(*desaturate_img).data[i].G = (*desaturate_img).data[i].R;
		(*desaturate_img).data[i].B = (*desaturate_img).data[i].R;
	}

	//Create decompose image
	(*decompose_img).magic_identifier = original_img.magic_identifier;
	(*decompose_img).width = original_img.width;
	(*decompose_img).height = original_img.height;
	(*decompose_img).max_value = original_img.max_value;
	(*decompose_img).data = malloc(sizeof(RGBpixel) * original_img.width * original_img.height);

	for (int i = 0; i < (*decompose_img).width*(*decompose_img).height; i++)
	{
		(*decompose_img).data[i].R = getmax(original_img.data[i].R, original_img.data[i].G, original_img.data[i].B);
		(*decompose_img).data[i].G = (*decompose_img).data[i].R;
		(*decompose_img).data[i].B = (*decompose_img).data[i].R;
	}


}

void Writeppm(PPMimage grayscale_img, PPMimage desaturate_img, PPMimage decompose_img)
{
	int counter;
	FILE *fp = fopen("grayscale_YenPham.ppm", "wb");
	FILE *fp1 = fopen("desaturate_YenPham.ppm", "wb");
	FILE *fp2 = fopen("decompose_YenPham.ppm", "wb");

	// Output a new grayscale image
	if (grayscale_img.magic_identifier == 3)
	{
		fprintf(fp, "P3\n");
		fprintf(fp, "%d %d %d\n", grayscale_img.width, grayscale_img.height, grayscale_img.max_value);

		counter = 1;
		for (int i = 0; i < grayscale_img.width* grayscale_img.height; i++)
		{
			fprintf(fp, "%d %d %d ", grayscale_img.data[i].R, grayscale_img.data[i].G, grayscale_img.data[i].B);

			if (counter == grayscale_img.width)
			{
				fprintf(fp, "\n");
				counter = 1;
			}
			else
			{
				counter++;
			}
		}
	}
	else if (grayscale_img.magic_identifier == 6)
	{
		fprintf(fp, "P6\n");
		fprintf(fp, "%d %d %d\n", grayscale_img.width, grayscale_img.height, grayscale_img.max_value);
		fwrite(grayscale_img.data, 3 * grayscale_img.width, grayscale_img.height, fp);
	}

	// Output a new desaturate image
	if (desaturate_img.magic_identifier == 3)
	{
		fprintf(fp1, "P3\n");
		fprintf(fp1, "%d %d %d\n", desaturate_img.width, desaturate_img.height, desaturate_img.max_value);

		counter = 1;
		for (int i = 0; i < desaturate_img.width* desaturate_img.height; i++)
		{
			fprintf(fp1, "%d %d %d ", desaturate_img.data[i].R, desaturate_img.data[i].G, desaturate_img.data[i].B);

			if (counter == desaturate_img.width)
			{
				fprintf(fp1, "\n");
				counter = 1;
			}
			else
			{
				counter++;
			}
		}
	}
	else if (desaturate_img.magic_identifier == 6)
	{
		fprintf(fp1, "P6\n");
		fprintf(fp1, "%d %d %d\n", desaturate_img.width, desaturate_img.height, desaturate_img.max_value);
		fwrite(desaturate_img.data, 3 * desaturate_img.width, desaturate_img.height, fp1);
	}

	// Output a new decompose image
	if (decompose_img.magic_identifier == 3)
	{
		fprintf(fp2, "P3\n");
		fprintf(fp2, "%d %d %d\n", decompose_img.width, decompose_img.height, decompose_img.max_value);

		counter = 1;
		for (int i = 0; i < decompose_img.width* decompose_img.height; i++)
		{
			fprintf(fp2, "%d %d %d ", decompose_img.data[i].R, decompose_img.data[i].G, decompose_img.data[i].B);

			if (counter == decompose_img.width)
			{
				fprintf(fp2, "\n");
				counter = 1;
			}
			else
			{
				counter++;
			}
		}
	}
	else if (decompose_img.magic_identifier == 6)
	{
		fprintf(fp2, "P6\n");
		fprintf(fp2, "%d %d %d\n", decompose_img.width, decompose_img.height, decompose_img.max_value);
		fwrite(decompose_img.data, 3 * decompose_img.width, decompose_img.height, fp2);
	}
}

int main()
{
	char infile[50];
	//size_t len = 500;
	char temp_line[500];
	char *temp_input;
	PPMimage original_img;
	PPMimage grayscale_img;
	PPMimage desaturate_img;
	PPMimage decompose_img;

	// Prompt user to enter ppm image
	do
	{
		printf("Enter ppm file name: ");
		scanf("%s", infile);
		if (strstr(infile, ".ppm") == NULL)
		{
			printf("Please input a .ppm file\n");
		}
	} while (strstr(infile, ".ppm") == NULL);
	
	// Call functions
	Readppm(infile, &original_img);
	Createppm(original_img, &grayscale_img, &desaturate_img, &decompose_img);
	Writeppm(grayscale_img, desaturate_img, decompose_img);
	return 0;
}