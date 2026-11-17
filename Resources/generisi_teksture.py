import sys
from PIL import Image, ImageDraw, ImageFont

out = sys.argv[1]
S = 256
FILL = (245, 245, 245, 255)
LINE = (110, 110, 110, 255)
W = 6


def save(img, name):
    img.save(f"{out}/{name}", "PNG")


# Majica
img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
d = ImageDraw.Draw(img)
shirt = [(80, 30), (105, 30), (115, 50), (141, 50), (151, 30), (176, 30), (240, 75), (210, 120),
         (190, 105), (190, 230), (66, 230), (66, 105), (46, 120), (16, 75)]
d.polygon(shirt, fill=FILL, outline=LINE, width=W)
d.arc((105, 10, 151, 60), 0, 180, fill=LINE, width=W)
save(img, "majica.png")

# Pantalone
img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
d = ImageDraw.Draw(img)
pants = [(70, 20), (186, 20), (200, 236), (140, 236), (128, 90), (116, 236), (56, 236)]
d.polygon(pants, fill=FILL, outline=LINE, width=W)
d.line([(70, 45), (186, 45)], fill=LINE, width=W)
d.line([(128, 45), (128, 90)], fill=LINE, width=4)
save(img, "pantalone.png")

# Peskir
img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
d = ImageDraw.Draw(img)
d.rectangle((40, 30, 216, 226), fill=FILL, outline=LINE, width=W)
for y in (60, 75, 181, 196):
    d.line([(40, y), (216, y)], fill=LINE, width=4)
save(img, "peskir.png")

# Carapa (crtana belo, u programu se boji u crveno mnozenjem sa bojom)
img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
d = ImageDraw.Draw(img)
sock = [(95, 20), (165, 20), (165, 160), (215, 190), (215, 236), (110, 236), (95, 200)]
d.polygon(sock, fill=FILL, outline=LINE, width=W)
d.line([(95, 50), (165, 50)], fill=LINE, width=W)
save(img, "carapa.png")

# Kursor - pakovanje omeksivaca (64x64, RGBA)
C = 64
img = Image.new("RGBA", (C, C), (0, 0, 0, 0))
d = ImageDraw.Draw(img)
d.rectangle((10, 2, 22, 12), fill=(240, 240, 240, 255), outline=(60, 60, 60, 255))        # cep
d.polygon([(8, 12), (24, 12), (30, 20), (4, 20)], fill=(90, 160, 230, 255), outline=(30, 60, 120, 255))  # vrat
d.rounded_rectangle((2, 18, 46, 62), radius=8, fill=(90, 160, 230, 255), outline=(30, 60, 120, 255), width=2)  # telo
d.rounded_rectangle((34, 24, 44, 50), radius=4, outline=(30, 60, 120, 255), width=2)     # rucka
d.rectangle((6, 30, 30, 52), fill=(255, 255, 255, 255))                                   # etiketa
d.ellipse((11, 34, 25, 48), fill=(250, 150, 200, 255))                                     # cvet na etiketi
save(img, "kursor.png")

# Natpis sa imenom, prezimenom i indeksom
img = Image.new("RGBA", (512, 128), (0, 0, 0, 180))
d = ImageDraw.Draw(img)
font = ImageFont.truetype("C:/Windows/Fonts/arialbd.ttf", 40)
d.text((20, 14), sys.argv[2], font=font, fill=(255, 255, 255, 255))
d.text((20, 68), sys.argv[3], font=font, fill=(255, 255, 255, 255))
save(img, "potpis.png")
print("ok")
