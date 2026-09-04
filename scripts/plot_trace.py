import argparse as ap
import matplotlib.colors as mcolours
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


COLOURS = [
    mcolours.TABLEAU_COLORS['tab:green'],
    mcolours.TABLEAU_COLORS['tab:brown']
]


def preprocess_trace(df: pd.Dataframe) -> pd.Dataframe:
    # construct frame id
    current_frame: int = 1
    frame_ids: [int] = []

    for label in df['label']:
        frame_ids.append(current_frame)

        if label.lower() == "present":
            current_frame += 1

    df.insert(0, "frame id", frame_ids)

    # Time scaling
    df['start (s)'] = df['start (ns)'] / 1_000_000
    df['duration (ms)'] = df['duration (ns)'] / 1_000

    # moving average
    df['duration moving average (ms)'] = (
        df["duration (ms)"]
        .rolling(window=10, min_periods=1)
        .mean()
    )

    return df


def plot_frametime_heatmap(df: pd.Dataframe, savefile: str, ftypes: list[str]):
    pass


def plot_frametime_line(df: pd.Dataframe, savefile: str, ftypes: list[str]):

    for label in LABEL_NAMES:
        samples = df[df["label"] == label]

        ax = samples.plot(
            title=f"Frametime (ms) - [{label.upper()}]",
            xlabel="Simulation time (s)",
            ylabel="Frametime (ms)",
            x='start (s)',
            y=['duration (ms)', 'duration moving average (ms)'],
            color=COLOURS,
            grid=True,
            figsize=(20, 8)
        )

        ax.legend(['Frametime', 'Moving Avg.'])

        for ftype in ftypes:
            fname: str = f"{savefile}-{label.lower()}.{ftype}"
            ax.figure.savefig(fname, bbox_inches='tight')


def plot_frametime_trace(df: pd.Dataframe, savefile: str, ftypes: list[str]):

    df = preprocess_trace(df)

    plot_frametime_line(df, savefile, ftypes)
    plot_frametime_heatmap(df, savefile, ftypes)


def csv_as_dataframe(path: str) -> pd.Dataframe:
    return pd.read_csv(path)


if __name__ == '__main__':
    argparser = ap.ArgumentParser(
        prog="arcxel trace plotter",
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

        plot_frametime_trace(df, ofile, ftypes)
