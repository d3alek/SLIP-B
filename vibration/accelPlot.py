import serial
import pygame
from pygame.locals import *
import sys

s = serial.Serial("/dev/ttyUSB0", 38400, timeout=0) 

def main():
    incomplete_line = []
    bufferedLines = []
    pygame.init()

    xs = []
    ys = []
    zs = []

    windowSurfaceObj = pygame.display.set_mode((800, 800));
    while True:
        windowSurfaceObj.fill((255,255,255)); 
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
                x = (int)(float(ss[0])*10)
                y = (int)(float(ss[1])*10)
                z = (int)(float(ss[2])*10)

        if x != 0 or y != 0 or z != 0:
            print x, y, z
            xs += [x]
            ys += [y]
            zs += [z]
        
        for i in range(0, len(xs) - 1):
            x = i * 10;
            nextx = (i + 1) * 10
            pygame.draw.line(windowSurfaceObj, (255, 0, 0),  (x, xs[i]), (nextx, xs[i+1]), 2);  
            pygame.draw.line(windowSurfaceObj, (0, 255, 0),  (x, ys[i]), (nextx, ys[i+1]), 2);  
            pygame.draw.line(windowSurfaceObj, (0, 0, 255),  (x, zs[i]), (nextx, zs[i+1]), 2);  
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit();
                sys.exit()
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    pygame.event.post(pygame.event.Event(QUIT))
           
        pygame.display.update();
        
if __name__=='__main__':
    main()
