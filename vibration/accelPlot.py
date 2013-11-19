import serial
import pygame
from pygame.locals import *
import sys

from sys import path as syspath
syspath.append(syspath[0] + '/python_lib')

s = serial.Serial("/dev/ttyUSB2", 38400, timeout=0) 

SCALEX = 1
SCALEY = 10 

def main():
    
    SAMPLE_FREQ = 1
    incomplete_line = []
    bufferedLines = []
    pygame.init()
    
    sampleCounter = 0

    xs = []
    ys = []
    zs = []

    windowSurfaceObj = pygame.display.set_mode((800, 800));
    
    background = pygame.Surface((800, 800))
    background.fill((255,255,255))

    zeroXPos = 200
    zeroYPos = 400
    zeroZPos = 600

    pygame.draw.circle(background, (0, 0, 0), (6, zeroXPos), 3, 1)
    pygame.draw.line(background, (0, 0, 0), (6, zeroXPos - SCALEY-1), (6, zeroXPos - SCALEY+1), 1)
    pygame.draw.line(background, (0, 0, 0), (6, zeroXPos + SCALEY-1), (6, zeroXPos + SCALEY+1), 1)
    pygame.draw.circle(background, (0, 0, 0), (6, zeroYPos), 3, 1)
    pygame.draw.line(background, (0, 0, 0), (6, zeroYPos - SCALEY-1), (6, zeroYPos - SCALEY+1), 1)
    pygame.draw.line(background, (0, 0, 0), (6, zeroYPos + SCALEY-1), (6, zeroYPos + SCALEY+1), 1)
    pygame.draw.circle(background, (0, 0, 0), (6, zeroZPos), 3, 1)
    pygame.draw.line(background, (0, 0, 0), (6, zeroZPos - SCALEY-1), (6, zeroZPos - SCALEY+1), 1)
    pygame.draw.line(background, (0, 0, 0), (6, zeroZPos + SCALEY-1), (6, zeroZPos + SCALEY+1), 1)

    while True:
        windowSurfaceObj.blit(background, (0,0))
        x = y = z = 0
        lines = s.readlines()
        if len(lines) > 0:
            if incomplete_line:
                lines[0] = incomplete_line + lines[0] 
            if lines[-1][-1] != '\n':
                incomplete_line = lines[-1]
                lines = lines[:-1]
            else:
                incomplete_line = None
            bufferedLines.extend(lines)

        if len(bufferedLines) > 0:
            line = bufferedLines[0]
            bufferedLines = bufferedLines[1:]
            ss = line.split()
            print line
            print ss
            if len(ss) is not 3:
               print "problem ss len is not 3!" 
            else:
                try:
                    x = zeroXPos - (int)(float(ss[0])*SCALEY)
                    if x < 100:
                        continue
                    y = zeroYPos - (int)(float(ss[1])*SCALEY)
                    z = zeroZPos - (int)(float(ss[2].split('.')[0])*SCALEY)
                except ValueError:
                    print "parsing error"
                    x = y = z = 0

        if x != 0 or y != 0 or z != 0:
            sampleCounter += 1
            if sampleCounter < SAMPLE_FREQ:
                continue
            else:
                sampleCounter = 0
            print x, y, z
            xs += [x]
            ys += [y]
            zs += [z]
        
        for i in range(0, len(xs) - 1):
            x = i * SCALEX;
            nextx = (i + 1) * SCALEX
            pygame.draw.line(windowSurfaceObj, (255, 0, 0),  (x, xs[i]), (nextx, xs[i+1]), 2);  
            pygame.draw.line(windowSurfaceObj, (0, 255, 0),  (x, ys[i]), (nextx, ys[i+1]), 2);  
            pygame.draw.line(windowSurfaceObj, (0, 0, 255),  (x, zs[i]), (nextx, zs[i+1]), 2);  
        
        if len(xs) > 800/SCALEX:
            mid = 800/(SCALEX*2)
            xs = xs[mid:]
            ys = ys[mid:]
            zs = zs[mid:]

        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit();
                sys.exit()
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    pygame.event.post(pygame.event.Event(QUIT))
                if event.key == K_UP:
                    if SAMPLE_FREQ < 10:
                        SAMPLE_FREQ += 1
                    print "SAMPLE_FREQ", SAMPLE_FREQ
                if event.key == K_DOWN:
                    if SAMPLE_FREQ >= 0:
                        SAMPLE_FREQ -= 1
                    print "SAMPLE_FREQ", SAMPLE_FREQ

        pygame.display.update();
        
if __name__=='__main__':
    main()
