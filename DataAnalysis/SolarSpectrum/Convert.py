import matplotlib.pyplot as plt
import pandas as pd
import json
import numpy as np

AM15Spectrum = pd.read_csv("DataAnalysis/SolarSpectrum/astmg173.csv", skiprows=1)

cols = AM15Spectrum.columns

print(AM15Spectrum)

h = 6.626e-34
c = 299792458

AM15Spectrum[cols[1]] = AM15Spectrum[cols[1]] / ((h * c)/(AM15Spectrum[cols[0]] * 1e-9))
AM15Spectrum[cols[2]] = AM15Spectrum[cols[2]] / ((h * c)/(AM15Spectrum[cols[0]] * 1e-9))
AM15Spectrum[cols[3]] = AM15Spectrum[cols[3]] / ((h * c)/(AM15Spectrum[cols[0]] * 1e-9))

Wavelength = AM15Spectrum[cols[0]]
N = AM15Spectrum[cols[3]]
dLambda = np.empty_like(Wavelength)
dLambda[:-1] = np.diff(Wavelength)
dLambda[-1] = dLambda[-2]

weights = N * dLambda
pdf = weights / weights.sum()

plt.figure(figsize=(16, 10))
plt.plot(Wavelength, pdf)
plt.show()

data = {}

data["Name"] = "AM1.5G Solar Spectrum Probability Distribution"
data["Wavelength"] = Wavelength.astype(float).tolist()
data["Probability"] = pdf.astype(float).tolist()

with open("DataAnalysis/SolarSpectrum/AM15_spectrum.json", "w") as f:
    json.dump(data, f, indent=4)