import pandas as pd
import matplotlib.pyplot as plt
import argparse

def plot_elo_ratings(models, elos, games_per_gen, output):
    games_played = [int(m.split("_")[1].split(".")[0]) * games_per_gen for m in models]
    games_played, elos = zip(*sorted(zip(games_played, elos), key=lambda x: x[0]))

    plt.figure(figsize=(10, 6))
    plt.plot(games_played, elos, marker="o", linestyle="-")
    plt.title("Deep Wallwars Elo Progression")
    plt.xlabel("Total Games Played")
    plt.ylabel("Elo Rating")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output)

parser = argparse.ArgumentParser()
parser.add_argument("file", help="Path to elo csv file as generated by bayeselo")
parser.add_argument("--output", help="Path to output", default="elo_progression.png")
parser.add_argument("--games", help="Number of games per iteration", default=5000, type=int)
args = parser.parse_args()

elo_df = pd.read_csv(args.file, sep=r"\s+")
plot_elo_ratings(elo_df["Name"], elo_df["Elo"], args.games, args.output)
