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


def plot_trace(df: pd.Dataframe, savefile: str, ftypes: list[str]) -> None:

    for label in LABEL_NAMES:
        samples = df[df["label"] == label]
        samples.sort_values('start (ns)', inplace=True)
        samples['start (ns)'] = samples['start (ns)'] / 1_000_000
        samples['duration (ns)'] = samples['duration (ns)'] / 1_000

        ax = samples.plot(
            title=f"Frametime (ms) - [{label.upper()}]",
            xlabel="Simulation time (s)",
            ylabel="Frametime (ms)",
            x='start (ns)',
            y='duration (ns)',
            figsize=(20, 5)
        )

        ax.legend([label])

        for ftype in ftypes:
            ax.figure.savefig(f"{savefile}-{label.lower()}.{ftype}")


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
        help='Directory to store plot images',
        default=os.path.curdir)

    argparser.add_argument(
        '-t',
        '--ftypes',
        nargs='+',
        help='File types to save charts as. Default: png',
        default=['png'])

    args = argparser.parse_args()

    infiles: [str] = args.files
    outdir: str = args.outdir
    ftypes: [str] = args.ftypes

    if not os.path.exists(outdir):
        print(f"Output directory '{outdir}' does not exit. Creating...")
        os.mkdir(outdir)

    csvs = list(map(csv_as_dataframe, infiles))

    for idx, df in enumerate(csvs):
        ifname: str = infiles[idx]
        ofname: str = os.path.basename(ifname.split('.')[0])
        ofile: str = os.path.join(outdir, ofname)

        plot_trace(df, ofile, ftypes)
