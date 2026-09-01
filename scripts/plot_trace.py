import argparse as ap
import pandas as pd

import os


LABEL_NAMES = [
    'Frame',
    'Events',
    'Update',
    'Construct',
    'Draw',
    'Present',
    'Render',
]


def plot_trace(df: pd.Dataframe, savefile: str) -> None:

    for label in LABEL_NAMES[0:1]:
        samples = df[df["label"] == label]
        samples.sort_values('start (ns)', inplace=True)

        ax = samples.plot(
            title="Frametime (ms)",
            xlabel="Simulation time (s)",
            ylabel="Frametime (ms)",
            x='start (ns)',
            y='duration (ns)'
        )

        ax.figure.savefig(f"{savefile}-{label.lower()}.svg")
        ax.figure.savefig(f"{savefile}-{label.lower()}.png")


def csv_as_dataframe(path: str) -> pd.Dataframe:
    return pd.read_csv(path)


if __name__ == '__main__':
    argparser = ap.ArgumentParser(
        prog="trace plotter",
        description="Plot Arcxel trace files")

    argparser.add_argument(
        'files',
        nargs='+',
        help='Trace files to plot')

    argparser.add_argument(
        '-o',
        '--outdir',
        help='directory to store plot images',
        default=os.path.curdir)

    args = argparser.parse_args()

    infiles: [str] = args.files
    outdir: str = args.outdir

    csvs = list(map(csv_as_dataframe, infiles))[0:1]

    for idx, df in enumerate(csvs):
        ifname: str = infiles[idx]
        ofname: str = os.path.basename(ifname.split('.')[0])

        plot_trace(df, os.path.join(outdir, ofname))
