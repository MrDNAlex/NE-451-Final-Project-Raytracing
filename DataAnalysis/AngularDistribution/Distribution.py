import numpy as np
import json
import matplotlib.pyplot as plt
import pandas as pd

def GenerateGaussianDistribution(sigma, mu, showPlots = True):

    x = np.linspace(-sigma*2, sigma*2, 10000)

    gaussian = (1/(sigma*np.sqrt(2*np.pi)))*np.exp(-0.5*((x-mu)/sigma)**2)

    if showPlots:
        plt.figure(figsize=(16, 10))
        plt.plot(x, gaussian)
        plt.show()

    delta = x[1] - x[0]

    weights = gaussian * delta
    weights = weights / weights.sum()

    if weights.sum() == 1:
        print("Sum is 1")

    if showPlots:
        plt.figure(figsize=(16, 10))
        plt.plot(x, weights)
        plt.show()

    data = {}
    data["Name"] = "Angle Normal Distribution"
    data["Angle"] = x.tolist()
    data["Probability"] = weights.tolist()

    with open(f"DataAnalysis/AngularDistribution/Distributions/{sigma*2}DegGaussian.json", "w") as f:
        json.dump(data, f, indent=4)

for i in range(5, 45, 5):
    GenerateGaussianDistribution(i, 0, False)
