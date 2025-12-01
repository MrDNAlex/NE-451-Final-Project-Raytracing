import numpy as np
import matplotlib.pyplot as plt

# Define Geometry Parameters
patternHeight = 250
hexagonT= 250
sphereRad = 250
slices = 100

# PDMS Sellmeier Equation
B1 = 1.0093
C1 = 13185
sellmeierPDMS = lambda x: np.sqrt(1+(B1*x**2)/(x**2-C1))

# Define Wavelength and Height Vectors
height = np.linspace(0, patternHeight, slices)
wavelength = np.linspace(250, 900, 10000)

# Get Colours
cmap = plt.get_cmap("magma")
colors = [cmap(i/slices) for i in range(slices)]

plt.figure(figsize=(16, 10))
index = 0
for h in height:

    # Calculate Geometry Areas
    hexagonArea = 0.5*3*np.sqrt(3) * hexagonT**2
    circleArea = np.pi*np.sqrt(2*sphereRad*h-h**2)**2

    # Calculate the Fill Factor Percentage
    fillFactor = circleArea/hexagonArea
    print(fillFactor)

    q = 2.0/3.0
    
    nEff = ((fillFactor*sellmeierPDMS(wavelength)**q) + ((1-fillFactor)*1**q))**(1/q)
    plt.plot(wavelength, nEff, label=f"Height % : {h*100}", color=colors[index])
    index+=1

#plt.legend()
plt.show()
plt.close()