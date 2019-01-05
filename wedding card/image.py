import pandas as pd;
from math import isnan;
from subprocess import call;
from PIL import Image;
from PIL import ImageFont;
from PIL import ImageDraw;
import pyperclip;

data = pd.read_csv("/Users/rushikesh/Documents/Didi wedding/wedding/invitees.csv",skiprows=0);
# font = ImageFont.truetype(<font-file>, <font-size>)
font = ImageFont.truetype("../Gujarati Sangam MN.ttc", size=38,index=1);
sp_img_orig = Image.open("page1.jpg");
def save_to_file(imageobj,drawobj,text,out_file):
	# draw.text((x, y),"Sample Text",(r,g,b))
	drawobj.text((410, 465),text,(0,100,0),font=font) #dark green
	imageobj.save(out_file);


names = data.iloc[:,1]; #names in column 2
seq=data.iloc[:,0]; #sequence numbers in column 1
numbers_panda=data.iloc[:,2]; #phone numbers in column 3

index=int(327);

while index < len(names):
	sp_img=sp_img_orig.copy();
	sp_draw = ImageDraw.Draw(sp_img);
	if isnan(numbers_panda[index]): #if number field empty
		print('\033[31mInvalid NaN\033[0m');
		index+=1;
		continue;

	number = int(numbers_panda[index]);
	no_str = str(number);
	
	
	name=names[index];
	pyperclip.copy(name);
	print(index,name,no_str);
	save_to_file(sp_img , sp_draw,name,"names/"+str(int(seq[index]))+" "+name+".jpg");
	index+=1;
	sp_img.close();
	op=input("whatsapp?");
	if(op=='n' or op=='N'):
		index+=1;
		continue;

	call(['electron','/Users/rushikesh/wa.js',no_str]);
	a=input("continue...");
	if(a=='n' or a=='N'):
		break;
sp_img_orig.close();