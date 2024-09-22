import math
import random
import matplotlib.pyplot as plt


"""
[1, 2, 3, 4, 5, A, 6, 7, 8, 9]

'A' marks the end of A truck route, rest of values B truck route

"""


def crossover(parent1, parent2):
    """ Function to create children from the trading of genes from 2 parent lists
    :return 2 children lists"""
    mutate = 0.2
    child1 = [None] * len(parent1)
    child2 = [None] * len(parent1)
    # Randomly chose the substring of genes to swap
    x1 = int(random.random() * len(parent1))
    x2 = int(random.random() * len(parent1))
    start = min(x1, x2)
    end = max(x1, x2)
    # Swap the substring genes
    for i in range(start, end):
        child1[i] = parent2[i]
        child2[i] = parent1[i]
    # Fill the rest of the child1 list with non repeating values
    j = 0
    for i in range(len(parent1)):
        # Check if index is already set
        if child1[i] is None:
            # Check if value is already used in child
            while parent1[j] in child1:
                j += 1
            child1[i] = parent1[j]
    # Fill the rest of the child2 list with non repeating values
    j = 0
    for i in range(len(parent1)):
        if child2[i] is None:
            while parent2[j] in child2:
                j += 1
            child2[i] = parent2[j]

    # Check if either child randomly mutates
    #if random.random() <= mutate:
    #    child1 = mutation(child1)
    #if random.random() <= mutate:
    #    child2 = mutation(child2)

    return child1, child2


def mutation(child):
    """ Function to swap some values in child chromosomes
    :return mutated chromosome array"""
    swap1 = int(random.random() * len(child))
    swap2 = int(random.random() * len(child))
    # Make sure values aren't equivalent
    while swap1 == swap2:
        swap2 = int(random.random() * len(child))
    temp = child[swap1]
    child[swap1] = child[swap2]
    child[swap2] = temp
    return child


def fitness(chromosome, houses):
    """ Function to measure the fitness of a given chromosome
    by finding the total distance traveled by  delivery trucks
    :return float fitness"""
    # sqrt((x1 - x2)^ + (y1 - y2)^2)
    # Distance from warehouse to first house in route
    start = houses['A']
    end = houses[chromosome[0]]
    totalDistance = math.sqrt((start[0] - end[0]) ** 2 + (start[1] - end[1]) ** 2)
    # Distance between houses in route
    for i in range(len(chromosome) - 1):
        # Marks the end of the A truck route, start the B truck route
        if chromosome[i] == 'A':
            start = houses['B']
        else:
            start = houses[chromosome[i]]
        end = houses[chromosome[i + 1]]
        totalDistance += math.sqrt((start[0] - end[0]) ** 2 + (start[1] - end[1]) ** 2)
    # Distance between last house in route and warehouse
    start = houses[chromosome[-1]]
    end = houses['B']
    totalDistance += math.sqrt((start[0] - end[0]) ** 2 + (start[1] - end[1]) ** 2)
    # Want to minimize distance, inverse of totalDistance is fitness
    return 1 / totalDistance


def fitnessRatio(routeFitness):
    """ :return list of probabilty that route is selected for crossover"""
    totalFitness = sum(routeFitness)
    probability = []
    for route in routeFitness:
        probability.append(route / totalFitness)
    return probability


if __name__ == '__main__':
    numHouses = 45
    mapSize = 35
    numChromosomes = 10
    numGenerations = 2000
    mutateProb = 0.30
    # 10 simulated runs
    for simulation in range(10):
        chromosomes = []
        routeFitness = []
        houses = {'A': (10, 5), 'B': (25, 30)}
        houseList = ['A']
        avgFitness = []
        # Generate random houses
        for i in range(numHouses):
            houses.update({i: (int(random.random() * mapSize), int(random.random() * mapSize))})
            houseList.append(i)
        # Generate random chromosomes
        for i in range(numChromosomes):
            chromosomes.append(random.sample(houseList, numHouses))

        for route in chromosomes:
            routeFitness.append(fitness(route, houses))
        probability = fitnessRatio(routeFitness)

        avgFitness.append(sum(routeFitness) / numChromosomes)

        for i in range(numGenerations):
            parent1 = []
            parent2 = []
            oldGen = chromosomes.copy()
            print(chromosomes)
            print(oldGen)
            probability = fitnessRatio(routeFitness)
            print(probability)

            # Create children chromosomes
            while len(chromosomes) < 2 * numChromosomes:
                # Crossover
                if random.random() > mutateProb:
                    # Roulette wheel selection of parents
                    parent1 = random.choices(oldGen, weights=probability)[0]
                    parent2 = random.choices(oldGen, weights=probability)[0]
                    # Make sure the parent doesn't repeat
                    # while parent1 == parent2:
                    #    parent2 = random.choices(oldGen, weights=probability)[0]
                    child1, child2 = crossover(parent1, parent2)
                    chromosomes.append(child1)
                    routeFitness.append(fitness(child1, houses))
                    chromosomes.append(child2)
                    routeFitness.append(fitness(child2, houses))
                # Mutation
                else:
                    child1 = mutation(random.choices(oldGen)[0])
                    chromosomes.append(child1)
                    routeFitness.append(fitness(child1, houses))

            # Trim least fit chromosomes from population
            while len(chromosomes) > numChromosomes:
                unfit = routeFitness.index(min(routeFitness))
                del routeFitness[unfit]
                del chromosomes[unfit]
            print(i)
            avgFitness.append(sum(routeFitness) / numChromosomes)
            print(sum(routeFitness))

        print(chromosomes)
        plt.plot(avgFitness)
        plt.xlabel("Generations")
        plt.title("Average fitness")
        plt.show()
