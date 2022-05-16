/*
===============================================================================
 Name        : DrawLine.c
 Author      : $RJ
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <math.h>




/* Be careful with the port number and location number, because

some of the location may not exist in that port. */

#define PORT_NUM            0


uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];


#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29



#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

// defining color values

#define YELLOW 0xFFFF00
#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF


int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;

#define SwitchPinNumber 2
#define LedPinNumber 3
#define ThreeBitMask   0x07

void GPIOinitOut(uint8_t portNum, uint32_t pinNum)
{
	if (portNum == 0)
	{
		LPC_GPIO0->FIODIR |= (1 << pinNum);
	}
	else if (portNum == 1)
	{
		LPC_GPIO1->FIODIR |= (1 << pinNum);
	}
	else if (portNum == 2)
	{
		LPC_GPIO2->FIODIR |= (1 << pinNum);
	}
	else
	{
		puts("Not a valid port!\n");
	}
}

void GPIOinitIn(uint8_t portNum, uint32_t pinNum)
{
	if (portNum == 0)
	{
		LPC_GPIO0->FIODIR |= (0 << pinNum);
	}
	else if (portNum == 1)
	{
		LPC_GPIO1->FIODIR |= (0 << pinNum);
	}
	else if (portNum == 2)
	{
		LPC_GPIO2->FIODIR |= (0 << pinNum);
	}
	else
	{
		puts("Not a valid port!\n");
	}
}

void setGPIO(uint8_t portNum, uint32_t pinNum)
{
	if (portNum == 0)
	{
		LPC_GPIO0->FIOSET = (1 << pinNum);
		printf("Pin 0.%d has been set.\n",pinNum);
	}
	//Can be used to set pins on other ports for future modification
	else
	{
		puts("Only port 0 is used, try again!\n");
	}
}

//Deactivate the pin
void clearGPIO(uint8_t portNum, uint32_t pinNum)
{
	if (portNum == 0)
	{
		LPC_GPIO0->FIOCLR = (1 << pinNum);
		printf("Pin 0.%d has been cleared.\n", pinNum);
	}
	//Can be used to clear pins on other ports for future modification
	else
	{
		puts("Only port 0 is used, try again!\n");
	}
}

void led_init()
{
	printf("LED Demo Begins\n");

	GPIOinitOut(0, 6);
	GPIOinitOut(0, 7);
	GPIOinitOut(0, 8);
	GPIOinitOut(0, 9);
}

void spiwrite(uint8_t c)

{

	int pnum = 0;

	src_addr[0] = c;

	SSP_SSELToggle( pnum, 0 );

	SSPSend( pnum, (uint8_t *)src_addr, 1 );

	SSP_SSELToggle( pnum, 1 );

}



void writecommand(uint8_t c)
{
	LPC_GPIO0->FIOCLR |= (0x1<<21);

	spiwrite(c);
}



void writedata(uint8_t c)
{
	LPC_GPIO0->FIOSET |= (0x1<<21);

	spiwrite(c);
}



void writeword(uint16_t c)
{
	uint8_t d;

	d = c >> 8;

	writedata(d);

	d = c & 0xFF;

	writedata(d);
}



void write888(uint32_t color, uint32_t repeat)
{
	uint8_t red, green, blue;

	int i;

	red = (color >> 16);

	green = (color >> 8) & 0xFF;

	blue = color & 0xFF;

	for (i = 0; i< repeat; i++) {

		writedata(red);

		writedata(green);

		writedata(blue);

	}
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	writecommand(ST7735_CASET);

	writeword(x0);

	writeword(x1);

	writecommand(ST7735_RASET);

	writeword(y0);

	writeword(y1);
}

void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{
	int16_t i, width, height;

	width = x1-x0+1;

	height = y1-y0+1;

	setAddrWindow(x0,y0,x1,y1);

	writecommand(ST7735_RAMWR);

	write888(color,width*height);

}


void lcddelay(int ms)
{

	int count = 24000;

	int i;

	for ( i = count*ms; i--; i > 0);

}



void lcd_init()
{
	int i;
	printf("LCD Demo Begins!!!\n");
// Set pins P0.16, P0.21, P0.22 as output
	LPC_GPIO0->FIODIR |= (0x1<<16);

	LPC_GPIO0->FIODIR |= (0x1<<21);

	LPC_GPIO0->FIODIR |= (0x1<<22);

// Hardware Reset Sequence
	LPC_GPIO0->FIOSET |= (0x1<<22);
	lcddelay(500);

	LPC_GPIO0->FIOCLR |= (0x1<<22);
	lcddelay(500);

	LPC_GPIO0->FIOSET |= (0x1<<22);
	lcddelay(500);

// initialize buffers
	for ( i = 0; i < SSP_BUFSIZE; i++ )
	{

		src_addr[i] = 0;
		dest_addr[i] = 0;

	}

// Take LCD display out of sleep mode
	writecommand(ST7735_SLPOUT);
	lcddelay(200);

// Turn LCD display on
	writecommand(ST7735_DISPON);
	lcddelay(200);

}




void drawPixel(int16_t x, int16_t y, uint32_t color)
{

	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))

		return;

	setAddrWindow(x, y, x + 1, y + 1);

	writecommand(ST7735_RAMWR);

	write888(color, 1);

}



/*****************************************************************************


** Descriptions:        Draw line function

**

** parameters:           Starting point (x0,y0), Ending point(x1,y1) and color

** Returned value:        None

**

*****************************************************************************/


void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{

	int16_t slope = abs(y1 - y0) > abs(x1 - x0);

	if (slope) {

		swap(x0, y0);

		swap(x1, y1);

	}

	if (x0 > x1) {

		swap(x0, x1);

		swap(y0, y1);

	}

	int16_t dx, dy;

	dx = x1 - x0;

	dy = abs(y1 - y0);

	int16_t err = dx / 2;

	int16_t ystep;

	if (y0 < y1) {

		ystep = 1;

	}
	else {

		ystep = -1;

	}

	for (; x0 <= x1; x0++) {

		if (slope) {

			drawPixel(y0, x0, color);

		}

		else {

			drawPixel(x0, y0, color);

		}

		err -= dy;

		if (err < 0) {

			y0 += ystep;

			err += dx;

		}

	}

}

int round(float x)
{
	int ans = x * 10;
	if(ans % 10 >= 5){
		return (int)x + 1;
	}
	else{
		return (int)x;
	}
}

/*

 Main Function main()

*/

void square_rotate(int* a, int* b, float lamda, int color, int layer)
{
	int tmp_a;
	int tmp_b;
	int flag = 0;
	if(layer == 0){
		return;
	}
	for(int i = 0; i < 4; i++){
		if(flag == 0){
			tmp_a = a[i];
			tmp_b = b[i];
			flag = 1;
		}
		if(i == 3){
			a[i] = a[i] + lamda * (tmp_a - a[i]);

			b[i] = b[i] + lamda * (tmp_b - b[i]);
			break;
		}
		a[i] = a[i] + lamda * (a[i + 1] - a[i]);

		b[i] = b[i] + lamda * (b[i + 1] - b[i]);

	}
	drawLine(a[0],b[0],a[1],b[1],color);
	drawLine(a[2],b[2],a[1],b[1],color);
	drawLine(a[0],b[0],a[3],b[3],color);
	drawLine(a[2],b[2],a[3],b[3],color);
	square_rotate(a, b, lamda, color, layer - 1);
}



int rand_num(int a, int b)
{
	int ans = rand()%(b - a + 1) + a;
	return ans;
}

int rand_color(int n)
{
	if(n == 1){
		return LIGHTBLUE;
	}
	else if(n == 2){
		return GREEN;
	}
	else if(n == 3){
		return DARKBLUE;
	}
	else if(n == 4){
		return BLACK;
	}
	else if(n == 5){
		return BLUE;
	}
	else if(n == 6){
		return RED;
	}
	else if(n == 7){
		return MAGENTA;
	}
	else if(n == 8){
		return WHITE;
	}
	else if(n == 9){
		return PURPLE;
	}
	else{
		return 0;
	}
	return 0;
}

struct point_coordinate{
	int x;
	int y;
};

int cam_x = 140;
int cam_y = 140;
int cam_z = 140;

int light_x = 40;
int light_y = 60;
int light_z = 220;

struct point_coordinate world_to_viewer_coord (int x_w, int y_w, int z_w)
{
	int scrn_x, scrn_y, Dist=100, x_diff=ST7735_TFTWIDTH/2, y_diff=ST7735_TFTHEIGHT/2;
	double x_p, y_p, z_p, theta, phi, rho;
	struct point_coordinate screen;
	theta = acos(cam_x/sqrt(pow(cam_x,2)+pow(cam_y,2)));
	phi = acos(cam_z/sqrt(pow(cam_x,2)+pow(cam_y,2)+pow(cam_z,2)));
	//theta = 0.785;
	//phi = 0.785;
	rho = sqrt((pow(cam_x,2))+(pow(cam_y,2))+ (pow(cam_z,2)));
	x_p = (y_w*cos(theta))- (x_w*sin(theta));
	y_p = (z_w*sin(phi))- (x_w*cos(theta)*cos(phi))- (y_w*cos(phi)*sin(theta));
	z_p = rho- (y_w*sin(phi)*cos(theta))- (x_w*sin(phi)*cos(theta))- (z_w*cos(phi));
	scrn_x = x_p*Dist/z_p;
	scrn_y = y_p*Dist/z_p;
	scrn_x = x_diff+scrn_x;
	scrn_y = y_diff-scrn_y;
	screen.x = scrn_x;
	screen.y = scrn_y;
	return screen;
}

void draw_coordinates ()
{
	struct point_coordinate lcd;
	int x1,y1,x2,y2, x3,y3,x4,y4;
	lcd = world_to_viewer_coord (0,0,0);
		x1=lcd.x;
		y1=lcd.y;
	lcd = world_to_viewer_coord (180,0,0);
		x2=lcd.x;
		y2=lcd.y;
	lcd = world_to_viewer_coord (0,180,0);
		x3=lcd.x;
		y3=lcd.y;
	lcd = world_to_viewer_coord (0,0,180);
		x4=lcd.x;
		y4=lcd.y;

	drawLine(x1,y1,x2,y2,RED);	//x axis red
	drawLine(x1,y1,x3,y3,GREEN);	//y axis green
	drawLine(x1, y1, x4, y4,BLUE);	//z axis blue
}

void draw_HorizontalLine(int16_t x, int16_t y, int16_t width, uint32_t color)
{
	drawLine(x, y, x+width-1, y, color);
}

void fill_Triangle(int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color) {
	int16_t x, y, j, l;
	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}
	if (y1 > y2) {
		swap(y2, y1);
		swap(x2, x1);
	}
	if (y0 > y1) {
		swap(y0, y1);
		swap(x0, x1);
	}
	if(y0 == y2) {
		x = y = x0;
		if(x1 < x) x = x1;
		else if(x1 > y) y = x1;

		if(x2 < x) x = x2;
		else if(x2 > y) y = x2;
		draw_HorizontalLine(x,y0, y-x+1, color);
		return;
	}

	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1;
	int32_t sa = 0, sb = 0;

	if(y1 == y2) l = y1;
	else l = y1-1;

	for(j=y0; j<=l; j++) {
		x = x0 + sa / dy01; y = x0 + sb / dy02; sa += dx01;
		sb += dx02;
		if(x > y) swap(x,y);
		draw_HorizontalLine(x, j, y-x+1, color);
	}
	sa = dx12 * (j - y1); sb = dx02 * (j - y0);
	for(; j<=y2; j++) {
		x = x1 + sa / dy12; y = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if(x > y) swap(x,y); draw_HorizontalLine(x, j, y-x+1, color);
	}
}


void draw_cube(int start_pnt_x, int start_pnt_y, int start_pnt_z, int size)
{
	struct point_coordinate lcd;
	int x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,x6,y6,x7,y7,x8,y8;
	cam_x = 200;
	cam_y = 200;
	cam_z = 200;

	lcd = world_to_viewer_coord (start_pnt_x,start_pnt_y,(size+start_pnt_z));
		x1=lcd.x;
		y1=lcd.y;
	lcd = world_to_viewer_coord ((size+start_pnt_x),start_pnt_y,(size+start_pnt_z));
		x2=lcd.x;
		y2=lcd.y;
	lcd = world_to_viewer_coord ((size+start_pnt_x),(size+start_pnt_y),(size+start_pnt_z));
		x3=lcd.x;
		y3=lcd.y;
	lcd = world_to_viewer_coord (start_pnt_x,(size+start_pnt_y),(size+start_pnt_z));
		x4=lcd.x;
		y4=lcd.y;
	lcd = world_to_viewer_coord ((size+start_pnt_x),start_pnt_y,start_pnt_z);
		x5=lcd.x;
		y5=lcd.y;
	lcd = world_to_viewer_coord ((size+start_pnt_x),(size+start_pnt_y), start_pnt_z);
		x6=lcd.x;
		y6=lcd.y;
	lcd = world_to_viewer_coord (start_pnt_x,(size+start_pnt_y),start_pnt_z);
		x7=lcd.x;
		y7=lcd.y;
	lcd = world_to_viewer_coord (start_pnt_x,start_pnt_y,start_pnt_z);
		x8=lcd.x;
		y8=lcd.y;
	drawLine(x1, y1, x2, y2,BLACK);
	drawLine(x2, y2, x3, y3,BLACK);
	drawLine(x3, y3, x4, y4,BLACK);
	drawLine(x4, y4, x1, y1,BLACK);
	drawLine(x2, y2, x5, y5,BLACK);
	drawLine(x5, y5, x6, y6,BLACK);
	drawLine(x6, y6, x3, y3,BLACK);
	drawLine(x6, y6, x7, y7,BLACK);
	drawLine(x7, y7, x4, y4,BLACK);
	drawLine(x5, y5, x8, y8,BLACK);
	drawLine(x7, y7, x8, y8,BLACK);
	drawLine(x1, y1, x8, y8,BLACK);
}

double reflectivity[3] = {0.9, 0, 0};

int create(double r, double g, double b)
{
	return (((int)r & 0xFF)<<16)+(((int)g & 0xFF)<<8)+((int) b&0xFF);
}

#define display_scaling (41300.5)
#define display_shifting (-155.5)

double calculate_intensity(int16_t xPs, int16_t yPs, int16_t zPs, int16_t xPi, int16_t yPi, int16_t zPi, int16_t i) {
	double   cosVal;
	double r = sqrt( pow((zPs - zPi), 2) + pow((yPs - yPi), 2) + pow((xPs - xPi), 2));
	double rcos = sqrt(pow((zPs - zPi), 2));
	cosVal = rcos / r;
	return (reflectivity[i] * 255 * cosVal) / pow(r, 2);
}

//-------------DDA Algorithm for drawLine----------------------------
void drawLineDDA(int16_t x0, int16_t y0, int16_t x1, int16_t y1, double kR1, double kR2)
{
	int dx, dy, steps, i;
	float xInc, yInc, x = x0, y = y0;
	double n = 0;

	dx = x1 - x0;
	dy = y1 - y0;

	if(fabs(dx) > fabs(dy)){
		steps = fabs(dx);
	}
	else{
		steps = fabs(dy);
	}

	n = (kR2 - kR1) / steps;

	drawPixel(round(x), round(y), create(kR1, 0, 0));

	xInc = (float)dx / (float)steps;
	yInc = (float)dy / (float)steps;

	for(i = 0; i < steps; i++){
		x += xInc;
		y += yInc;
		drawPixel(round(x), round(y), create((kR1 + n), 0, 0));
	}
}


void diffused_reflection(int size)
{
	struct point_coordinate tmp, c1, c2, c3, c4;

	double c_kR[4];
	uint32_t c_color[4];
//-------------DDA Algorithm for boundary----------------------------
	c_kR[0] = calculate_intensity(light_x, light_y, light_z, 0, 0, size, 0);
	c_kR[1] = calculate_intensity(light_x, light_y, light_z, size, 0, size, 0);
	c_kR[2] = calculate_intensity(light_x, light_y, light_z, 0, size, size, 0);
	c_kR[3] = calculate_intensity(light_x, light_y, light_z, size, size, size, 0);

	for(int i = 0; i < 4; i++){
		c_kR[i] = c_kR[i] * display_scaling + display_shifting;
		if(c_kR[i] >= 255){
			c_kR[i] = 255;
		}
		else if(c_kR[i] <= 20){
			c_kR[i] = 20;
		}
	}

	c1 = world_to_viewer_coord(0, 0, size + 10);
	c2 = world_to_viewer_coord(size, 0, size + 10);
	c3 = world_to_viewer_coord(size, size, size + 10);
	c4 = world_to_viewer_coord(0, size, size + 10);

	drawLineDDA(c1.x, c1.y, c2.x, c2.y, c_kR[0], c_kR[1]);
	drawLineDDA(c3.x, c3.y, c2.x, c2.y, c_kR[1], c_kR[2]);
	drawLineDDA(c3.x, c3.y, c4.x, c4.y, c_kR[2], c_kR[3]);
	drawLineDDA(c1.x, c1.y, c4.x, c4.y, c_kR[3], c_kR[0]);
//-------------Linear for interior top----------------------------
	for (int i = 1; i < size; i++){
		for (int j = 1; j < size; j++){
			tmp = world_to_viewer_coord(i, j, size + 10);
			double kR = calculate_intensity(light_x, light_y, light_z, i, j, size, 0);
			kR = kR * display_scaling + display_shifting;
			if(kR >= 255){
				kR = 255;
			}
			else if(kR <= 20){
				kR = 20;
			}

			uint32_t color;
			color = create(kR, 0, 0);

			drawPixel(tmp.x, tmp.y, color);
		}
	}
//-------------Linear for interior left----------------------------
	for (int i = 0; i <= size; i++){
		for (int j = 10; j <= size + 10; j++){
			tmp = world_to_viewer_coord (size,i,j);
			double kR = calculate_intensity(light_x, light_y, light_z, size, i, j, 0);
			kR = kR * display_scaling + display_shifting;
			if(kR >= 255){
				kR = 255;
			}
			else if(kR <= 20){
				kR = 20;
			}

			uint32_t color;
			color = create(kR, 0, 0);

			drawPixel(tmp.x, tmp.y, color);
		}
	}
//-------------Linear for interior right----------------------------
	for (int i = 0; i <= size; i++)
	{
		for (int j = 10; j <= size + 10; j++) {
			tmp = world_to_viewer_coord (i,size,j);
			double kR = calculate_intensity(light_x, light_y, light_z, i, size, j, 0);
			kR = kR * display_scaling + display_shifting;
			if(kR >= 255){
				kR = 255;
			}
			else if(kR <= 20){
				kR = 20;
			}
			uint32_t color;
			color = create(kR, 0, 0);

			drawPixel(tmp.x, tmp.y, color);
		}
	}
}

void shadow(double x[], double y[], double z[])
{
	int light_src_x = 40;
	int light_src_y = 60;
	int light_src_z = 220;
	cam_x = 200;
	cam_y = 200;
	cam_z = 200;

	int x_its[4]={0}, y_its[4]={0}, z_its[4]={0};
	struct point_coordinate s1,s2,s3,s4,l1;
	float lambda = (-1 * light_src_z) / (light_src_z - z[6]);
//-------------[6] = (size, size, size)----------------------------
	x_its[0] = light_src_x + lambda * (light_src_x - x[6]);
	y_its[0] = light_src_y + lambda * (light_src_y - y[6]);
	z_its[0] = light_src_z + lambda * (light_src_z - z[6]);
//-------------[5] = (size, 0, size)----------------------------
	x_its[1] = light_src_x + lambda * (light_src_x - x[5]);
	y_its[1] = light_src_y + lambda * (light_src_y - y[5]);
	z_its[1] = light_src_z + lambda * (light_src_z - z[5]);
//-------------[7] = (0, size, size)----------------------------
	x_its[2] = light_src_x + lambda * (light_src_x - x[7]);
	y_its[2] = light_src_y + lambda * (light_src_y - y[7]);
	z_its[2] = light_src_z + lambda * (light_src_z - z[7]);
//-------------[4] = (0, 0, size)----------------------------
	x_its[3] = light_src_x + lambda * (light_src_x - x[4]);
	y_its[3] = light_src_y + lambda * (light_src_y - y[4]);
	z_its[3] = light_src_z + lambda * (light_src_z - z[4]);

	s1 = world_to_viewer_coord(x_its[0],y_its[0],z_its[0]);
	s2 = world_to_viewer_coord(x_its[1],y_its[1],z_its[1]);
	s3 = world_to_viewer_coord(x_its[2],y_its[2],z_its[2]);
	s4 = world_to_viewer_coord(x_its[3],y_its[3],z_its[3]);

	l1 = world_to_viewer_coord(light_src_x,light_src_y,light_src_z);

//----------------shadow-------------------------
	drawLine(s1.x, s1.y, s2.x, s2.y, BLACK);
	drawLine(s2.x, s2.y, s4.x, s4.y, BLACK);
	drawLine(s3.x, s3.y, s4.x, s4.y, BLACK);
	drawLine(s3.x, s3.y, s1.x, s1.y, BLACK);
//---------------fill shadow-------------------------
	fill_Triangle(s1.x, s1.y, s2.x, s2.y, s4.x, s4.y,BLACK);
	fill_Triangle(s1.x, s1.y, s3.x, s3.y, s4.x, s4.y,BLACK);
//--------------light to shadow---------------------
	drawLine(l1.x, l1.y, s1.x, s1.y, YELLOW);
	drawLine(l1.x, l1.y, s2.x, s2.y, YELLOW);
	drawLine(l1.x, l1.y, s3.x, s3.y, YELLOW);
	drawLine(l1.x, l1.y, s4.x, s4.y, YELLOW);
}

//-------------K on top----------------------------
void draw_K(int start_x, int start_y, int start_z, int size, uint32_t color)
{
	int xs[6], ys[6], z = start_z + size;
	int xss[6], yss[6];
	int x, y;
	struct point_coordinate p0, p1, p2, p3, p4, p5;

	xs[0] = start_x + 10;
	ys[0] = start_y + 10;

	xs[1] = start_x + size - size / 3;
	ys[1] = ys[0];

	xs[2] = xs[1];
	ys[2] = ys[1] + size/3;

	xs[3] = xs[0];
	ys[3] = ys[2];

	xs[4] = xs[0];
	ys[4] = ys[3] + size/3;

	xs[5] = xs[1];
	ys[5] = ys[4];

	for(int i = -5; i < 6; i++){
		p0 = world_to_viewer_coord(xs[0] + i, ys[0], z);
		p4 = world_to_viewer_coord(xs[4] + i, ys[4], z);
		drawLine(p0.x, p0.y, p4.x, p4.y, color);
	}

	for(int i = -5; i < 6; i++){
		p3 = world_to_viewer_coord(xs[3], ys[3] - i, z);
		p5 = world_to_viewer_coord(xs[5] + i, ys[5], z);
		drawLine(p3.x, p3.y, p5.x, p5.y, color);
	}

	for(int i = -5; i < 6; i++){
		p1 = world_to_viewer_coord(xs[1] + i, ys[1], z);
		p3 = world_to_viewer_coord(xs[3], ys[3] + i, z);
		drawLine(p1.x, p1.y, p3.x, p3.y, color);
	}
}
//-------------3D square----------------------------
void square_rotate_3D(int start_px, int* s_y, int* s_z, float lamda, int color, int layer, int size)
{
	struct point_coordinate p1, p2, p3, p4;
	int x = start_px + size;

	if(layer == 0){
		return;
	}

	p1 = world_to_viewer_coord(x, s_y[0], s_z[0]);
	p2 = world_to_viewer_coord(x, s_y[1], s_z[1]);
	p3 = world_to_viewer_coord(x, s_y[2], s_z[2]);
	p4 = world_to_viewer_coord(x, s_y[3], s_z[3]);

	drawLine(p1.x,p1.y,p2.x,p2.y,color);
	drawLine(p2.x,p2.y,p3.x,p3.y,color);
	drawLine(p3.x,p3.y,p4.x,p4.y,color);
	drawLine(p4.x,p4.y,p1.x,p1.y,color);

	int tmp_x = 0, tmp_y = 0, flag = 0;

	for(int i = 0; i < 4; i++){
		if(flag == 0){
			tmp_x = s_y[0];
			tmp_y = s_z[0];
			flag = 1;
		}
		if(i == 3){
			s_y[i] = s_y[i] + lamda * (tmp_x - s_y[i]);
			s_z[i] = s_z[i] + lamda * (tmp_y - s_z[i]);
			break;
		}
		s_y[i] = s_y[i] + lamda * (s_y[i + 1] - s_y[i]);
		s_z[i] = s_z[i] + lamda * (s_z[i + 1] - s_z[i]);
	}
	square_rotate_3D(start_px, s_y, s_z, lamda, color, layer - 1, size);
}

typedef struct Point
{
	float x;
	float y;
}Point;

#define PI 3.1415926
//-------------3D tree----------------------------
Point rotate_point(Point start, Point end, float angle)
{
	Point post, rotate, pre;
	angle = angle * (PI/180);
	float sina = sinf(angle);
	float cosa = cosf(angle);
	//post
	post.x = start.x - end.x;
	post.y = start.y - end.y;
	//rotate
	rotate.x = post.x * cosa + post.y * -sina;
	rotate.y = post.x * sina + post.y * cosa;
	//pre
	pre.x = rotate.x + end.x;
	pre.y = rotate.y + end.y;
	return pre;
}

void tree_branch_3D(Point start, Point end, int y, float lamda, float angle, int level)
{
	struct point_coordinate d_m, d_l, d_r, d_e;
	Point mid, left, right;

	if(level == 0){
		return;
	}
	d_e = world_to_viewer_coord(end.x, y, end.y);
	//mid branch
	mid.x = end.x + lamda * (end.x - start.x);
	mid.y = end.y + lamda * (end.y - start.y);
	d_m = world_to_viewer_coord(end.x + lamda * (end.x - start.x), y, end.y + lamda * (end.y - start.y));

	drawLine(d_m.x, d_m.y, d_e.x, d_e.y, GREEN);

	//right branch
	right = rotate_point(mid, end, angle);
	d_r = world_to_viewer_coord(right.x, y, right.y);
	drawLine(d_r.x, d_r.y, d_e.x, d_e.y, GREEN);

	//left branch
	left = rotate_point(mid, end, 360 - angle);
	d_l = world_to_viewer_coord(left.x, y, left.y);
	drawLine(d_l.x, d_l.y, d_e.x, d_e.y, GREEN);

	tree_branch_3D(end, mid, y,lamda, angle, level - 1);
	tree_branch_3D(end, right, y, lamda, angle, level - 1);
	tree_branch_3D(end, left, y, lamda, angle, level - 1);
}

void tree_stem_3D(int start_px, int start_py, int start_pz, int end_px, int end_py, int end_pz, int color)
{
	struct point_coordinate ps, pe;
	ps = world_to_viewer_coord(start_px, start_py, start_pz);
	pe = world_to_viewer_coord(end_px, end_py, end_pz);
	drawLine(ps.x, ps.y, pe.x, pe.y, color);
}


#define SwitchPinNumber 2
#define LedPinNumber 3
#define ThreeBitMask   0x07

int main (void)
{

	int size = 100, start_pnt_x = 0, start_pnt_y = 0, start_pnt_z = 10;
	int start_pnt = 0;

	double x[8] = {start_pnt_x,(start_pnt_x+size),(start_pnt_x+size), start_pnt_x,       start_pnt_x,      (start_pnt_x+size),(start_pnt_x+size), start_pnt_x};
	double y[8] = {start_pnt_y, start_pnt_y,      (start_pnt_y+size),(start_pnt_y+size), start_pnt_y,       start_pnt_y,      (start_pnt_y+size),(start_pnt_y+size) };
	double z[8] = {start_pnt_z, start_pnt_z,       start_pnt_z,       start_pnt_z,      (start_pnt_z+size),(start_pnt_z+size),(start_pnt_z+size),(start_pnt_z+size)};
//                 start pnt    pnt5               pnt6               pnt7               pnt1             pnt2             pnt3             pnt4

	srand(time(0));

	 uint32_t pnum = PORT_NUM;

	 pnum = 0 ;

	 if ( pnum == 0 )
		 SSP0Init();

	 else
		 puts("Port number is not correct");

	 lcd_init();

	 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, WHITE);

	  uint32_t switchStatus;
	  SystemInit();                    //Clock and PLL configuration

	  GPIOinitIn(0, 2);
	  GPIOinitOut(0, 3);

	  draw_coordinates();

	  draw_cube(start_pnt_x, start_pnt_y, start_pnt_z, size);

	  shadow(x, y, z);

	  diffused_reflection(size);

	  draw_K(0 ,0 ,0, size, BLUE);

	  int s_y[4] = {20, 20, 20 + (size * 2/3), 20 + (size * 2/3)};
	  int s_z[4] = {30, 30 + (size * 2/3), 30 + (size * 2/3), 30};
	  square_rotate_3D(0, s_y, s_z, 0.8, WHITE, 5, size);

	  //tree_3D
	  Point t_s, t_e;
	  int length = 20;
	  t_s.x = size / 2;
	  t_s.y = start_pnt_z;
	  t_e.x = size / 2;
	  t_e.y = start_pnt_z + length;

	  tree_stem_3D(size / 2, size, start_pnt_z, size / 2, size, start_pnt_z + length, BLACK);
	  tree_branch_3D(t_s, t_e, size, 0.8, 30, 5);

	  return 0;

}
