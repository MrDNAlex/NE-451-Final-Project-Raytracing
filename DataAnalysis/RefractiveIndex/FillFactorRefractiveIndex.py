import numpy as np
import matplotlib.pyplot as plt
patternHeight = 250
hexagonT= 250
sphereRad = 250

B1 = 1.0093
C1 = 13185

sellmeierPDMS = lambda x: np.sqrt(1+(B1*x**2)/(x**2-C1))

y = np.linspace(0, patternHeight, 10)
wavelength = np.linspace(250, 4000, 10000)

height = (patternHeight - y)

plt.figure(figsize=(16, 10))

for h in height:
    newH = h/patternHeight
    nEff = newH*sellmeierPDMS(wavelength) + (1-newH)*1
    plt.plot(wavelength, nEff, label=f"Height % : {h*100}")

plt.show()
plt.close()

hexagonArea = 0.5*3*np.sqrt(3) * hexagonT**2

plt.figure(figsize=(16, 10))
for h in height:
    fillFactor = np.pi*np.sqrt(-h**2 +2*sphereRad*h)/hexagonArea
    print(fillFactor)

    q = 2.0/3.0
    nEff = ((fillFactor*sellmeierPDMS(wavelength))**q + ((1-fillFactor)*1)**q)**(1/q)
    plt.plot(wavelength, nEff, label=f"Height % : {h*100}")

plt.legend()
plt.show()
plt.close()
