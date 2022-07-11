#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp.h"

char default_filename[] = "upscaled.bmp";

void usage()
{
    printf("Usage: ./upscale_example <filename> <ouput_width> <output_height> [<output_filename>]\n");
}



static inline uint8_t *ptr_at(void *data, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
{
    if (x >= width || y >= height) { return 0; }
    return ((uint8_t *)data) + (x + y * width) * 3;
}
//输出像素点的地址，返回地址值

float BiCubic(float x)                             //双三次插值中的BiCubic函数，用于计算插值
{
    float x1,c = -0.75;
    x1 = fabs(x);
    if(x1 <= 1)
    {
        return (c+2)*x1*x1*x1 - (c+3)*x1*x1 + 1;
    }
    else if((x1 > 1) && (x1 < 2))
    {
        return c * x1 * x1 * x1 - 5 * c * x1 * x1 + 8 * c * x1 - 4 * c;
    }
    else
    {
        return 0;
    }
}



int upscale_example(void *data_in, uint32_t width_in, uint32_t height_in, void *data_out, uint32_t width_out, uint32_t height_out)
{
    uint32_t x,y,ch;
    //float width_times = 0.25;
    //float height_times = 0.25 ;

    for (y = 0; y < height_out; y++)
    {
        for (x = 0; x < width_out; x++)
        {
           
            //uint8_t *pix = ptr_at(data_out, width_out, height_out, x, y);                                                       //destination图片中P像素点的位置

           // float a = x * 0.25;                       
           // float b = y * 0.25;  
          // x = 34;
          // y = 43;
           float a = (x + 0.5) * 0.25 - 0.5; 
           float b = (y + 0.5) * 0.25 - 0.5;

           //float a = (x ) * 0.25 + 0.125; 
          // float b = (y ) * 0.25 + 0.125;
            uint32_t X = (int)a;                               
            uint32_t Y = (int)b;             
//            uint8_t *pix_c = ptr_at(data_out, width_out, height_out, x, y); 




        

            //当N点位于source图像内部时，使用三次插值处理
           
            //if (a > 1 && a < (width_in -2) && b > 1 && b < (height_in-2))  
            if (a > 1.5 && a < (width_in -2.5) && b > 1.5 && b < (height_in-2.5))                
            {
                uint8_t *pix_b = ptr_at(data_out, width_out, height_out, x, y); 
                //float v = a - X + 0.125;      
                //float u = b - Y + 0.125;   
                float v = a - X;
                float u = b - Y;

                float X1[4] , Y1[4];         

                float d0 = 1 + v;
                float d1 = v;
                float d2 = 1-v;
                float d3 = 2 - v;

                float d4 = 1 + u;
                float d5 = u;
                float d6 = 1 - u;
                float d7 = 2 - u;


                //计算双三次插值系数          
                X1[0] = BiCubic((float) ( d0 ));
                X1[1] = BiCubic((float) ( d1 ));
                X1[2] = BiCubic((float) ( d2 ));
                X1[3] = BiCubic((float) ( d3 ));

                Y1[0] = BiCubic((float) ( d4 ));
                Y1[1] = BiCubic((float) ( d5 ));
                Y1[2] = BiCubic((float) ( d6 ));
                Y1[3] = BiCubic((float) ( d7 ));



                //RGB用于存储16个点的像素值
                uint8_t R[4][4];  
                uint8_t G[4][4];  
                uint8_t B[4][4];   


                uint32_t x_abs,y_abs; 
                double W[4][4]; 
                double sum1 = 0;
                double sum2 = 0;
                double sum3 = 0;



                //计算16个点像素值并求和
                for (int i = 0;i <= 3;i++) 
                {              
                    for (int j = 0;j <= 3;j++)
                    {
                        x_abs = X-1+i;
                        y_abs = Y-1+j;
                        uint8_t *tmp=ptr_at(data_in, width_in, height_in, x_abs, y_abs);
                        R[i][j] = tmp[0];
                        G[i][j] = tmp[1];
                        B[i][j] = tmp[2];   

                        W[i][j] = X1[i]*Y1[j];     

                        sum1 += W[i][j]*R[i][j];
                        sum2 += W[i][j]*G[i][j];
                        sum3 += W[i][j]*B[i][j];

                    }   
                } 

                if (sum1 > 255)
                {
                    sum1 = 255;
                }
                else if (sum1 < 0)
                {
                    sum1 = 0;
                }
                else
                {
                    sum1 = sum1;
                }
                
                
                if (sum2 > 255)
                {
                    sum2 = 255;
                }
                else if (sum2 < 0)
                {
                    sum2 = 0;
                }
                else
                {
                    sum2 = sum2;
                }



                if (sum3 > 255)
                {
                    sum3 = 255;
                }
                else if (sum3 < 0)
                {
                    sum3 = 0;
                }
                else
                {
                    sum3 = sum3;
                }
/*
                for (int i = 0;i <= 3;i++) 
                {              
                    for (int j = 0;j <= 3;j++)
                    {
                        x_abs = X-1+i;
                        y_abs = Y-1+j;
                        uint8_t *tmp=ptr_at(data_in, width_in, height_in, x_abs, y_abs);
                        R[i][j] = tmp[0];
                        //G[i][j] = tmp[1];
                        //B[i][j] = tmp[2];   

                        W[i][j] = X1[i]*Y1[j];     
                        sum1 += W[i][j]*R[i][j];
                        //sum2 += W[i][j]*G[i][j];
                        //sum3 += W[i][j]*B[i][j];
                        if (sum1 > 255)
                        {
                            sum1 = 255;
                        }
                        else if (sum1 < 0)
                        {
                            sum1 = 0;
                        }
                    }   
                }

                for (int i = 0;i <= 3;i++)  
                {              
                    for (int j = 0;j <= 3;j++)
                    {
                        x_abs = X-1+i;
                        y_abs = Y-1+j;
                        uint8_t *tmp=ptr_at(data_in, width_in, height_in, x_abs, y_abs);
                        R[i][j] = tmp[0];
                        G[i][j] = tmp[1];
                        B[i][j] = tmp[2];   

                        W[i][j] = X1[i]*Y1[j];     

                        sum2 += W[i][j]*G[i][j];
                        sum2 += W[i][j]*G[i][j];
                        //sum3 += W[i][j]*B[i][j];
                        if (sum2 > 255)
                        {
                            sum2 = 255;
                        }
                        else if (sum2 < 0)
                        {
                            sum2 = 0;
                        }
                    }   
                }


                for (int i = 0;i <= 3;i++) 
                {              
                    for (int j = 0;j <= 3;j++)
                    {
                        x_abs = X-1+i;
                        y_abs = Y-1+j;
                        uint8_t *tmp=ptr_at(data_in, width_in, height_in, x_abs, y_abs);
                        //R[i][j] = tmp[0];
                        //G[i][j] = tmp[1];
                        B[i][j] = tmp[2];   

                        W[i][j] = X1[i]*Y1[j];     

                        sum3 += W[i][j]*B[i][j];
                        //sum2 += W[i][j]*G[i][j];
                        //sum3 += W[i][j]*B[i][j];
                        if (sum3 > 255)
                        {
                            sum3 = 255;
                        }
                        else if (sum3 < 0)
                        {
                            sum3 = 0;
                        }
                    }   
                }
*/


                pix_b[0] = (uint8_t)sum1;   
                pix_b[1] = (uint8_t)sum2; 
                pix_b[2] = (uint8_t)sum3;



                /*
                printf("sum1 = %d\n",sum1);
                printf("sum2 = %d\n",sum2);
                printf("sum3 = %d\n",sum3);
                */
               


            
/*
            for (int i = 0;i <= 3;i++) 
                {              
                    for (int j = 0;j <= 3;j++)
                    {
                        x_abs = X-1+i;
                        y_abs = Y-1+j;
                        uint8_t *tmp=ptr_at(data_in, width_in, height_in, x_abs, y_abs);
                        R[i][j] = tmp[0];
                        G[i][j] = tmp[1];
                        B[i][j] = tmp[2];   

                        W[i][j] = X1[i]*Y1[j];     

                        sum1 += W[i][j]*R[i][j];
                        sum2 += W[i][j]*G[i][j];
                        sum3 += W[i][j]*B[i][j];

                        
                    }   
                } 

*/

            }













            //最邻近插值
         //   else   if (a <= 1 || a > (width_in -2) || b <= 1 || b > (height_in-2))
            else {
                uint8_t *pix = ptr_at(data_out, width_out, height_out, x, y); 
                uint8_t *pix_a = ptr_at(data_in, width_in, height_in, X, Y);
                for (ch = 0; ch < 3; ch++)
                {
                    pix[ch] = pix_a[ch];
                }
            } 
          

        }   
    }
    
    
return 0;
}


int main(int argc, char const *argv[])
{
    if (argc < 4) {
        usage();
        exit(1);
    }
    
    BMPImage *img = bmp_read(argv[1]);
    BMPImage *res = bmp_create(atoi(argv[2]), atoi(argv[3]));
    upscale_example(img->data, img->header.width_px, img->header.height_px, res->data, res->header.width_px, res->header.height_px);

    return bmp_write(res, argc > 4 ? argv[4] : default_filename);
}