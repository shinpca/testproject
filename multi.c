/*4-I 15 齋藤新之助*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<ppm.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>

#define I_ERROR -5000

struct MatrixA_Factor{double x_scale;double y_scale;double turnR;double x_shear;
  double y_shear;double x_translation;double y_translation;};
struct MovingValue{double x;double y;};
struct point{int P_A[2];int P_B[2];int P_C[2];int P_D[2];};
struct test{double r;double g;double b;};
struct size{double cols;double rows;};


void F_Init_MxAFactor(struct MatrixA_Factor *MxAF);
void F_Make_MatrixA(struct MatrixA_Factor MatrixF,double (*MatrixA)[3]
		    ,int jswitch);
pixel F_bilinear(pixel **SW,int k,int l,double x,double y);
pixel **F_ImageProccesing(pixel **Source_Window,double (*MatrixA)[3],int *cols,
			  int *rows,int jswitch);
void F_Inverse_Matrix(double (*MatrixA)[3],double (*IMatrix)[3]);
int F_InverseM(double (*d_aMatrix)[3],double (*d_iMatrix)[3]);
struct size F_NewWindowSize(struct point data,double (*MatrixA)[3]);

void F_Cul_Matrix_33_33(double (*a)[3],double (*b)[3],double (*Answer)[3]);
void F_Cul_Matrix_33_31(double (*a)[3],double (*b)[1],double (*Answer)[1]);

struct MovingValue F_CooredinatesMoving(struct point data1,double (*MatrixA)[3]);
pixel F_NN_Interpolation(double u,double v,double (*IMatrix)[3],struct MovingValue CM,pixel **SW,struct point SWV);
pixel **F_PTranslation(pixel **NW,struct size NWS,int e,int f);
int F_Input(int a,int b);

void F_Substitute_MxAF(struct MatrixA_Factor MatrixF,double (*MatrixAF)[3],
		       int jswitch);
pixel F_bi_linear_Interpolation(double u,double v,double (*IMatrixA)[3],
				struct MovingValue CM,pixel **SW,struct size SWS);

struct size F_NewWindowSize(struct point SWindow_Vertex,double (*MatrixA)[3]);

void color_view(pixel **ppm_trans, int x, int y, int maxval);
int bit_count(unsigned int x);

int main(int argc, char *argv[])
{
    FILE *fp;
	int cols,rows,maxval,jswitch,temp_jswitch,SW_rows;
    pixel **Source_Window,**New_Window;
    double MatrixA[3][3]={{1.0,0,0},{0,1.0,0},{0,0,1.0}};
    struct MatrixA_Factor MxAFactor={1,1,0,0,0,0,0};

    if(argc == 2)
      fp = fopen(argv[1],"rb");
    else
      {
	printf("入力が不正です\n");
	exit(1);
      }
    if(fp == NULL)
      {
	printf("ファイルがありません\n");
	exit(1);
      }
    Source_Window = ppm_readppm(fp,&cols,&rows,&maxval);
    fclose(fp);
    while(1)
      {
	while(1)
	  {
	    printf("メニュー番号を選択してください\n1:線形変換\n2:アフィン変換\n");
	    jswitch = F_Input(1,2);
	if(jswitch == I_ERROR)
	  {
	    printf("入力が不正です\n");
	    continue;
	  }
	else break;
	  }
	temp_jswitch = jswitch;
	while(1)
	  {
	    switch(jswitch)
	      {
	      case 1:
		printf("メニュー番号を選択してください\n1:拡大縮小\n2:回転\n3:せん断\n");
		jswitch = F_Input(1,3);
		if(jswitch == I_ERROR)
		  {
		    printf("入力が不正です\n");
		    jswitch = temp_jswitch;
		    continue;
		  }
		else break;
	  
	      case 2:
		printf("メニュー番号を選択してください\n1:拡大縮小\n2:回転\n3:せん断\n4:平行移動\n");
		jswitch = F_Input(1,4);
		if(jswitch == I_ERROR)
		  {
		    printf("入力が不正です\n");
		    jswitch = temp_jswitch;
		    continue;
		  }
		else break;
	  
	      default :
		continue;
	      }
	    break;
	  }
	temp_jswitch = jswitch;
      while(1)
	{
	  F_Init_MxAFactor(&MxAFactor);
	  switch(jswitch)
	    {
	  
	    case 1:
	      printf("倍率を1〜500の整数で入力してください(単位％)\n");
	      printf("水平方向:");
	      MxAFactor.x_scale = (double)F_Input(1,500) / 100.0;
	      printf("鉛直方向:");
	      MxAFactor.y_scale = (double)F_Input(1,500) / 100.0;
	      if(MxAFactor.x_scale == I_ERROR/100 || MxAFactor.y_scale == I_ERROR/100)
		{
		  printf("入力が不正です\n");
		  jswitch = temp_jswitch;
		  continue;
		}
	      F_Make_MatrixA(MxAFactor,MatrixA,jswitch);
	      break;
	  
	    case 2:
	      printf("画像の左上を原点として反時計回りどれだけ回転するのかを入力してください(0°〜360°)\n");
	      MxAFactor.turnR = (double)F_Input(0,360);
	      if(MxAFactor.turnR==I_ERROR)
		{
		  printf("入力が不正です\n");
		  jswitch = temp_jswitch;
		  continue;
		}
	  F_Make_MatrixA(MxAFactor,MatrixA,jswitch);
	  break;
	  
		case 3:
		  printf("画像の左上を中心としてどれだけせん断するのかを入力してください(-89°〜89°)\n");
	 	 printf("水平方向:");
	 	 MxAFactor.x_shear=(double)F_Input(-89,89);
	 	 if(MxAFactor.x_shear==I_ERROR)
			{
	    		printf("入力が不正です\n");
	    		jswitch=temp_jswitch;
	    		continue;
	 		}
		F_Make_MatrixA(MxAFactor,MatrixA,jswitch);
		printf("鉛直方向:");
		F_Init_MxAFactor(&MxAFactor);
		MxAFactor.y_shear = (double)F_Input(-89,89);
		if(MxAFactor.y_shear==I_ERROR)
			{
		  		printf("入力が不正です\n");
	  			jswitch=temp_jswitch;
	  			continue;
			}
		F_Make_MatrixA(MxAFactor,MatrixA,jswitch);
		break;
	
		case 4:
	 	printf("画像の左上を中心としてどれだけ平行移動するのかを入力してください(-1000〜1000pixel)\n");
	  	printf("水平方向:");
	 	 MxAFactor.x_translation=(double)F_Input(-1000,1000);
	  	if(MxAFactor.x_translation==I_ERROR)
			{
	    		printf("入力が不正です\n");
	    		jswitch=temp_jswitch;
	    		continue;
	 		}
		F_Make_MatrixA(MxAFactor,MatrixA,jswitch);
		printf("鉛直方向:");
		F_Init_MxAFactor(&MxAFactor);
	    MxAFactor.y_translation=(double)F_Input(-1000,1000);
	    if(MxAFactor.y_translation==I_ERROR){
	    	printf("入力が不正です\n");
	    	jswitch=temp_jswitch;
	    	continue;
	 	}
	    F_Make_MatrixA(MxAFactor,MatrixA,jswitch);
	 	break;
	  
		default :
	  		continue;
	    }
	  break;
	}
      while(1)
	{
	  printf("選択した処理を実行しますか\n1:はい\n2:いいえ\n");
	  jswitch = F_Input(1,2);
	  if(jswitch == I_ERROR)
	    {
	      printf("入力が不正です\n");
	      continue;
	    }
	  break;
	}
      if(jswitch == 1) break;
      else continue;
      }
    while(1)
      {
	printf("メニュー番号を選択してください\n1:最近隣内挿法\n2：共１次内挿法\n");
	jswitch=F_Input(1,2);
	if(jswitch == I_ERROR)
	  {
	    printf("入力が不正です\n");
	    continue;
	  }
	break;
      }
    SW_rows = rows;
    New_Window = F_ImageProccesing(Source_Window,MatrixA,&cols,&rows,jswitch);
    ppm_freearray(Source_Window,SW_rows);
    while(1)
      {
	printf("出力形式番号を入力してください\n1:ディスプレイ\n2:ファイル\n");
	jswitch = F_Input(1,2);
	if(jswitch == I_ERROR)
	  {
	    printf("入力が不正です\n");
	    continue;
	  }
	break;
      }
    switch(jswitch)
      {
      case 1:
	color_view(New_Window,cols,rows,maxval);
	break;

      case 2:
	fp = fopen("New_Image.ppm","wb");
	ppm_writeppm(fp,New_Window,cols,rows,maxval,0);
	fclose(fp);
	break;
      }
    ppm_freearray(New_Window,rows);
}
struct MovingValue F_CooredinatesMoving(struct point data1,double (*MatrixA)[3])
{
  struct MovingValue data2;
  int i,A[2],B[2],C[2],D[2],max[2],min[2];
  double temp_data1[3][1]={0,0,0};
  double temp_data2[3][1]={0,0,0};

  temp_data1[0][0]=data1.P_A[0];
  temp_data1[1][0]=data1.P_A[1];
  temp_data1[2][0]=1;
  F_Cul_Matrix_33_31(MatrixA,temp_data1,temp_data2);

  A[0]=temp_data2[0][0];
  A[1]=temp_data2[1][0];
  temp_data1[0][0]=data1.P_B[0];
  temp_data1[1][0]=data1.P_B[1];
  temp_data1[2][0]=1;
  F_Cul_Matrix_33_31(MatrixA,temp_data1,temp_data2);

  B[0]=temp_data2[0][0];
  B[1]=temp_data2[1][0];
  temp_data1[0][0]=data1.P_C[0];
  temp_data1[1][0]=data1.P_C[1];
  temp_data1[2][0]=1;
  F_Cul_Matrix_33_31(MatrixA,temp_data1,temp_data2);

  C[0]=temp_data2[0][0];
  C[1]=temp_data2[1][0];
  temp_data1[0][0]=data1.P_D[0];
  temp_data1[1][0]=data1.P_D[1];
  temp_data1[2][0]=1;
  F_Cul_Matrix_33_31(MatrixA,temp_data1,temp_data2);

  D[0]=temp_data2[0][0];
  D[1]=temp_data2[1][0];
  
  for(i=0;i<=1;++i){
    if(A[i]>B[i]){
      min[i]=B[i];
    }
    else{
      min[i]=A[i];
    }
    if(min[i]>C[i]){
      min[i]=C[i];
    }
    if(min[i]>D[i]){
      min[i]=D[i];
    }
  }

  data2.x=min[0];
  data2.y=min[1];
  return (data2);
}

void F_Init_MxAFactor(struct MatrixA_Factor *MxAF)
{
  MxAF->x_scale = 1.0;
  MxAF->y_scale = 1.0;
  MxAF->turnR = 0;
  MxAF->x_shear = 0;
  MxAF->y_shear = 0;
  MxAF->x_translation = 0;
  MxAF->y_translation = 0;
}


void F_Make_MatrixA(struct MatrixA_Factor MatrixF,double (*MatrixA)[3],
		    int jswitch)
{
  int i,j;
  double MatrixAF[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
  double temp_MatrixA[3][3];
  for(i=0;i<=2;++i)
    {
      for(j=0;j<=2;++j)
	temp_MatrixA[i][j] = MatrixA[i][j];	
    }
  F_Substitute_MxAF(MatrixF,MatrixAF,jswitch);
  F_Cul_Matrix_33_33(MatrixAF,temp_MatrixA,MatrixA);
}

void F_Inverse_Matrix(double (*MatrixA)[3],double (*IMatrix)[3])
{
  int result;
  result = F_InverseM(MatrixA,IMatrix);
   if(result == -1)
     {
       exit(1);
     }
}


struct size F_NewWindowSize(struct point data1,double (*MatrixA)[3])
{
  struct size data2;
  int i;
  double temp_data1[3][1] = {0,0,1};
  double temp_data2[3][1] = {0,0,1};
  double A[2],B[2],C[2],D[2],max[2],min[2];

  temp_data1[0][0] = data1.P_A[0];
  temp_data1[1][0] = data1.P_A[1];
  temp_data1[2][0] = 1;
  F_Cul_Matrix_33_31(MatrixA,temp_data1,temp_data2);
  A[0]=temp_data2[0][0];
  A[1]=temp_data2[1][0];
  temp_data1[0][0] = data1.P_B[0];
  temp_data1[1][0] = data1.P_B[1];
  temp_data1[2][0] = 1;
  F_Cul_Matrix_33_31(MatrixA,temp_data1,temp_data2);
  B[0]=temp_data2[0][0];
  B[1]=temp_data2[1][0];
  temp_data1[0][0] = data1.P_C[0];
  temp_data1[1][0] = data1.P_C[1];
  temp_data1[2][0] = 1;
  F_Cul_Matrix_33_31(MatrixA,temp_data1,temp_data2);
  C[0]=temp_data2[0][0];
  C[1]=temp_data2[1][0];
  temp_data1[0][0] = data1.P_D[0];
  temp_data1[1][0] = data1.P_D[1];
  temp_data1[2][0] = 1;
  F_Cul_Matrix_33_31(MatrixA,temp_data1,temp_data2);
  D[0]=temp_data2[0][0];
  D[1]=temp_data2[1][0];
  
  for(i=0;i<=1;++i)
    {
    if(A[i] > B[i])
      {
      max[i] = A[i];
      min[i] = B[i];
      }
    else
      {
      max[i] = B[i];
      min[i] = A[i];
      }
    if(max[i] > C[i])
      {
      if(min[i] > C[i])
	{
	min[i] = C[i];
	}
      }
    else
      {
	max[i] = C[i];
      }
    if(max[i] > D[i])
      {
	if(min[i] > D[i])
	  {
	    min[i] = D[i];
	  }
      }
    else
      {
	max[i] = D[i];
      }
  }
  data2.cols = (double)floor((max[0] - min[0] + 1.0));
  data2.rows = (double)floor((max[1] - min[1] + 1.0));
  return (data2);
}

pixel F_NN_Interpolation(double u,double v,double (*IMatrix)[3],
			 struct MovingValue CM,pixel **SW,struct point SWV)
{
  pixel NW = {0,0,0};
  double NW_Matrix[3][1],Cul_uv_sw[3][1];

  u += CM.x;
  v += CM.y;
  NW_Matrix[0][0] = u;
  NW_Matrix[1][0] = v;
  NW_Matrix[2][0] = 1;
  F_Cul_Matrix_33_31(IMatrix,NW_Matrix,Cul_uv_sw);
  if(Cul_uv_sw[0][0] >= (0.0-1e-10) && Cul_uv_sw[1][0] >= (0.0-1e-10) 
     && Cul_uv_sw[0][0] <= ((double)SWV.P_C[0]+1e-10) && Cul_uv_sw[1][0] <= ((double)SWV.P_C[1]+1e-10)){
    NW = SW[(int)(Cul_uv_sw[1][0]+0.5)][(int)(Cul_uv_sw[0][0]+0.5)];
  }

  return NW;
}


pixel F_bi_linear_Interpolation(double u,double v,double (*IMatrixA)[3],
				struct MovingValue CM,pixel **SW ,struct size SWS)
{
  int k,l;
  double Cul_uv_sw[3][1];
  double NW_Matrix[3][1];

  pixel NW;
  
  NW.r = 0;
  NW.g = 0;
  NW.b = 0;
  if(u==180.0 && v==1.0)
	{
		printf("a");
	}
  u = u+CM.x;
  v = v+CM.y;
  NW_Matrix[0][0] = u;
  NW_Matrix[1][0] = v;
  NW_Matrix[2][0] = 1.0;

  F_Cul_Matrix_33_31(IMatrixA,NW_Matrix,Cul_uv_sw);
  if(Cul_uv_sw[0][0] >= (0.0-1e-10) && Cul_uv_sw[1][0] >= (0.0-1e-10)
     && Cul_uv_sw[0][0] <= (SWS.cols-1.0+1e-10) && Cul_uv_sw[1][0] <= (SWS.rows-1.0+1e-10))
	{
	    k = (int)Cul_uv_sw[0][0];
	    l = (int)Cul_uv_sw[1][0];
   		if(Cul_uv_sw[0][0] == (int)Cul_uv_sw[0][0] && Cul_uv_sw[1][0] == (int)Cul_uv_sw[1][0])
			{
    		  NW=SW[(int)Cul_uv_sw[1][0]][(int)Cul_uv_sw[0][0]];
			}
    	else
			{
      		  if((k<0 && k>-1) || (l<0 && l>-1) || (k+1>SWS.cols-1 && k+1<=SWS.cols) 
					|| (l+1>SWS.rows-1 && l+1<=SWS.rows))
				{
					if(k<0) k=-k;
					else{
						if(l<0) l=-l;
	 						 else{
	  							  if(k+1>SWS.cols-1) k-=1;
	  							  if(l+1>SWS.rows-1) l-=1;
	 							 }
						}
      		}
		NW=F_bilinear(SW,k,l,Cul_uv_sw[0][0],Cul_uv_sw[1][0]);
    }
  }
  return NW;
}

pixel **F_ImageProccesing(pixel **Source_Window,double (*MatrixA)[3],int *cols,
			  int *rows,int jswitch)
{
  struct point SWindow_Vertex={{0,0},{*cols-1,0},{*cols-1, *rows-1},{0, *rows-1}};
  int i,j,u,v;
  pixel **New_Window;
  struct size New_WindowSize,Source_WindowSize;
  struct MovingValue CooredinatesMoving;
  double IMatrixA[3][3],temp_MatrixA[3][3];

  for(i=0;i<=2;++i)
    {
      for(j=0;j<=2;++j)
	temp_MatrixA[i][j]=MatrixA[i][j];
    }
  F_Inverse_Matrix(MatrixA,IMatrixA);
  for(i=0;i<=2;++i)
    {
    for(j=0;j<=2;++j)
      MatrixA[i][j]=temp_MatrixA[i][j];
    }
  Source_WindowSize.cols = *cols;
  Source_WindowSize.rows = *rows;
  New_WindowSize = F_NewWindowSize(SWindow_Vertex,MatrixA);
  *cols = (int)New_WindowSize.cols;
  *rows = (int)New_WindowSize.rows;
  New_Window = ppm_allocarray(New_WindowSize.cols,New_WindowSize.rows);
  CooredinatesMoving = F_CooredinatesMoving(SWindow_Vertex,MatrixA);
  switch(jswitch)
    {
    case 1:
    for(v=0;v<New_WindowSize.rows;v++)
      {
      for(u=0;u<New_WindowSize.cols;u++)
	New_Window[v][u]=F_NN_Interpolation(u,v,IMatrixA,CooredinatesMoving,
					    Source_Window,SWindow_Vertex);
      }
    break;

    case 2:
   for(v=0;v<New_WindowSize.rows;v++)
     {
     for(u=0;u<New_WindowSize.cols;u++)
       New_Window[v][u]=F_bi_linear_Interpolation(u,v,IMatrixA,
						  CooredinatesMoving,Source_Window,Source_WindowSize);
     }
   break;
    }
  New_Window=F_PTranslation(New_Window,New_WindowSize,MatrixA[0][2],MatrixA[1][2]);
  return New_Window;
}






void F_Substitute_MxAF(struct MatrixA_Factor MatrixF,double (*MatrixAF)[3],int jswitch)
{
  if(jswitch == 4)
    {
      MatrixAF[0][2] = MatrixF.x_translation;
      MatrixAF[1][2] = MatrixF.y_translation;
    }
  else
    {
      if(jswitch == 2)
	{
      MatrixAF[0][0] = cos(MatrixF.turnR / 180.0 * M_PI);
      MatrixAF[0][1] = sin(MatrixF.turnR / 180.0 * M_PI);
      MatrixAF[1][0] = -sin(MatrixF.turnR / 180.0 * M_PI);
      MatrixAF[1][1] = cos(MatrixF.turnR / 180.0 * M_PI);
	}
      else
	{
	  if(jswitch == 3)
	    {
	      MatrixAF[0][1]=tan(MatrixF.x_shear / 180 * M_PI);
	      MatrixAF[1][0]=tan(MatrixF.y_shear / 180 * M_PI);
	    }
	  else
	    {
	      MatrixAF[0][0]=MatrixF.x_scale;
	      MatrixAF[1][1]=MatrixF.y_scale;
	    }
	}
    }
}




pixel F_bilinear(pixel **SW,int k,int l,double x,double y)
{
  double p[4],cal[4];
  pixel ans={0,0,0};
  int i,j,t=0;
  struct test temp[4];

  p[0] = (double)k+ 1.0 - x;
  p[1] = (double)l + 1.0 - y;
  p[2] = y- (double)l;
  p[3] = x - (double)k;

  cal[0] = p[0] * p[1];
  cal[2] = p[0] * p[2];
  cal[1] = p[1] * p[3];
  cal[3] = p[2] * p[3];

  for(i=l;i<=l+1;++i)
    {
      for(j=k;j<=k+1;++j)
	{
	  temp[t].r = SW[i][j].r*cal[t];
	  temp[t].g = SW[i][j].g*cal[t];
	  temp[t].b = SW[i][j].b*cal[t];
	  ++t;
    }
  }
  for(i=0;i<=3;++i){
    if(temp[i].r < 0) temp[i].r = 0;
    if(temp[i].g < 0) temp[i].g = 0;
    if(temp[i].b < 0) temp[i].b = 0;
  }
  ans.r = (gray)floor(temp[0].r + temp[1].r + temp[2].r + temp[3].r + 0.5);
  ans.g = (gray)floor(temp[0].g + temp[1].g + temp[2].g + temp[3].g + 0.5);
  ans.b = (gray)floor(temp[0].b + temp[1].b + temp[2].b + temp[3].b + 0.5);
  return (ans);
}




int F_Input(int a,int b)
{
  char c[100];
  int y,i,lengh;
  scanf("%s",c);
  lengh = strlen(c);
  for(i=0;i<lengh;++i)
    {
      if('0'<=c[i] && c[i]<='9' || c[i]=='-'){}
      else
	{
	  y=-5000;
	  return y;
	}
    }
  y = atoi(c);
  if(a<=y && y<=b)
    {}
  else y = -5000;
  return y;
}






pixel **F_PTranslation(pixel **NW,struct size NWS,int e,int f)
{
  int v,u,PTu,PTv;
  pixel **PT_NW,**temp_NW;

  temp_NW=NW;
  PT_NW=ppm_allocarray(NWS.cols,NWS.rows);
  for(v=0;v<NWS.rows;v++){
    for(u=0;u<NWS.cols;u++)
      {
	PTu=u+e;
	PTv=v+f;
	if(PTu < 0 || PTu > NWS.cols-1 || PTv < 0 || PTv > NWS.rows-1)
	  {}
	else
	  {
	    PT_NW[PTv][PTu] = NW[v][u];
	  }
      }
  }
  free (temp_NW);
  return PT_NW;
}







void F_Cul_Matrix_33_33(double (*a)[3],double (*b)[3],double (*Answer)[3])
{  
  int i,j,k;
  for(i=0;i<=2;++i)
    {
      for(j=0;j<=2;++j)
	Answer[i][j]=0;
    }
  for(i=0;i<=2;++i)
    {
      for(j=0;j<=2;++j)
	{
	  for(k=0;k<=2;k++)
	    Answer[i][j]+=a[i][k]*b[k][j];
	}
    }
}


void F_Cul_Matrix_33_31(double (*a)[3],double (*b)[1],double (*Answer)[1])
{
  int i,j;
  for(i=0;i<=2;++i)
    {
      Answer[i][0] = 0;
    }
  for(i=0;i<=2;++i)
    {
    for(j=0;j<=2;++j)
      {
	Answer[i][0] += a[i][j] * b[j][0];
      }
    }
}



int F_InverseM(double (*d_aMatrix)[3],double (*d_iMatrix)[3])
{
  int i,j,pv,pivot_row,row[3];
  double x,big,d_temp;
  for(i=1;i<=3;++i)
    {
      for(j=1;j<=3;++j)
	{
	  if(i==j)
	    {
	      d_iMatrix[i-1][j-1] = 1.0;
	    }
	  else{
	    d_iMatrix[i-1][j-1]=0.0;
	  }
	}
    }
  for(pv=1;pv<=3;++pv)
    {
      big = 0.0;
      for(i=pv;i<=3;++i)
	{
	  if(fabs(d_aMatrix[i-1][pv-1]) > big)
	    {
	      big = fabs(d_aMatrix[i-1][pv-1]);
	      pivot_row = i;
	    }
	}
      if(big == 0.0)
	return -1;

      if(pv != pivot_row)
	{
	  for(i=1 ; i<=3 ; ++i)
	    {
	      d_temp = d_aMatrix[pv-1][i-1];
	      d_aMatrix[pv-1][i-1] = d_aMatrix[pivot_row-1][i-1];
	      d_aMatrix[pivot_row-1][i-1] = d_temp;
	      d_temp = d_iMatrix[pv-1][i-1];
	      d_iMatrix[pv-1][i-1] = d_iMatrix[pivot_row-1][i-1];
	      d_iMatrix[pivot_row-1][i-1] = d_temp;
	    }
	}
  
      x=1.0 / d_aMatrix[pv-1][pv-1];
      for(j=1;j<=3;++j)
	{
	  d_aMatrix[pv-1][j-1] *= x;
	  d_iMatrix[pv-1][j-1] *= x;
	}
      for(i=1;i<=3;++i)
	{
	  if(i-1 != pv-1)
	    {
	      d_temp = d_aMatrix[i-1][pv-1];
	      for(j=1;j<=3;++j)
		{
		  d_aMatrix[i-1][j-1]-=d_temp*d_aMatrix[pv-1][j-1];
		  d_iMatrix[i-1][j-1]-=d_temp*d_iMatrix[pv-1][j-1];
		}
	    }
	}
    }
  return 0;
}



void Ft_OutputMatrix(double (*Matrix)[3])
{
  int i,j;
  for(i=1;i<=3;++i)
    {
      for(j=1;j<=3;++j)
	printf("%5g ",Matrix[i-1][j-1]);
      printf("\n");
    }
}


void color_view(pixel **ppm_trans, int x, int y, int maxval) {
  Display *disp;
  Window win;
  GC gc;
  Colormap cmap;
  XImage *im;
  XSetWindowAttributes at;
  XEvent ev;
  unsigned long *data;
  Visual *visual;
  int depth;

  data=(unsigned long *)malloc((size_t)x*y*sizeof(unsigned long));

  disp=XOpenDisplay(NULL);
  win=XCreateSimpleWindow(disp,DefaultRootWindow(disp),0,0,x,y,0,
			  BlackPixel(disp,0),WhitePixel(disp,0));
  at.override_redirect = False;
  at.save_under=True;
  at.backing_store=Always;
  XChangeWindowAttributes(disp,win,
			  CWOverrideRedirect|CWSaveUnder|CWBackingStore,&at);
  XSelectInput(disp,win,ExposureMask|ButtonReleaseMask);
  XMapWindow(disp,win);

  gc=XCreateGC(disp,win,0,0);
  cmap=DefaultColormap(disp,0);

  visual=DefaultVisual(disp,0);
  depth=DefaultDepth(disp,0);

  im=XCreateImage(disp,visual,depth,ZPixmap,0,0,0,0,32,0);
  im->byte_order=LSBFirst;
  im->bitmap_unit=32;
  im->bitmap_pad=32;
  im->bytes_per_line=x*sizeof(unsigned long);
  im->bits_per_pixel=8*sizeof(unsigned long);
  im->width=x;
  im->height=y;
  im->data=(char *)data;

  int i,j;

  int r_bit,g_bit,b_bit;

  r_bit=bit_count(visual->red_mask);
  g_bit=bit_count(visual->green_mask);
  b_bit=bit_count(visual->blue_mask);


  for(i=0;i<y;i++) {
    for(j=0;j<x;j++) {
      data[i*x+j]=((ppm_trans[i][j].r>>(8-r_bit))<<(g_bit+b_bit)
		   |((ppm_trans[i][j].g>>(8-g_bit))<<b_bit)
		   |(ppm_trans[i][j].b>>(8-b_bit)));
    }
  }

  XPutImage(disp,win,gc,im,0,0,0,0,x,y);
  XFlush(disp);

  do {
    XNextEvent(disp,&ev);

    if(ev.type == Expose) {
      XPutImage(disp,win,gc,im,0,0,0,0,x,y);
      XFlush(disp);
    }

  } while(ev.type != ButtonRelease);

  XDestroyWindow(disp,win);
  XCloseDisplay(disp);
}

int bit_count(unsigned int x) {
  x=x-((x>>1)&0x55555555);
  x=(x&0x33333333)+((x>>2)&0x33333333);
  x=(x+(x>>4))&0x0f0f0f0f;
  x=x+(x>>8);
  x=x+(x>>16);

  return x & 0x0000003f;
}

