import numpy as np
import matplotlib.pyplot as plt

# Define Geometry Parameters
patternHeight = 1
hexagonT= patternHeight*2/np.sqrt(3)
sphereRad = 1
slices = 100

# PDMS Sellmeier Equation
B1_PDMS = 1.0093
C1_PDMS = 13185
sellmeierPDMS = lambda x: np.sqrt(1+(B1_PDMS*x**2)/(x**2-C1_PDMS))

# Air Sellmeier Equation
B1_Air = 0.05792105
B2_Air = 0.00167917
C1_Air = 238.0185 
C2_Air = 57.362

sellmeierAir = lambda x: 1 + B1_Air/(C1_Air - (1/x**2)) + B2_Air/(C2_Air - (1/x**2))

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
    nEff = ((fillFactor*sellmeierPDMS(wavelength)**q) + ((1-fillFactor)*sellmeierAir(wavelength/1000)**q))**(1/q)
    plt.plot(wavelength, nEff, label=f"Height % : {h*100}", color=colors[index])
    index+=1

#plt.legend()
plt.show()
plt.close()