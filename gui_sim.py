"""
gui_sim.py — Tabla de simulación de colas
Lee sim_data.csv y actualiza la tabla cada 500ms.
Ejecutar: python gui_sim.py
"""

import tkinter as tk
from tkinter import ttk
import csv
import os
import sys

CSV_PATH = "sim_data.csv"
POLL_MS  = 500   # intervalo de refresco

# ── Colores ──────────────────────────────────────────────────────
BG        = "#0f1923"
SURFACE   = "#162030"
HEADER_BG = "#1e3a5f"
ROW_ODD   = "#131f2e"
ROW_EVEN  = "#0f1923"
FG        = "#cdd9e5"
FG_HEAD   = "#ffffff"
ACCENT    = "#00c8ff"
GREEN     = "#3ddc84"
YELLOW    = "#ffbd2e"
RED       = "#ff5f57"
FONT_BODY = ("Consolas", 10)
FONT_HEAD = ("Consolas", 10, "bold")
FONT_TITL = ("Consolas", 9, "bold")

class SimGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("SimQueue — Monitor")
        self.root.configure(bg=BG)
        self.root.geometry("1100x520")

        self.last_mtime = -1
        self.columns    = []
        self.tree       = None

        # ── Header ───────────────────────────────────────────────
        top = tk.Frame(root, bg=BG, pady=6)
        top.pack(fill="x", padx=12)

        tk.Label(top, text="SIM // QUEUE", font=("Consolas", 14, "bold"),
                 bg=BG, fg=ACCENT).pack(side="left")

        self.status_var = tk.StringVar(value="⏳ esperando datos...")
        tk.Label(top, textvariable=self.status_var, font=FONT_BODY,
                 bg=BG, fg=YELLOW).pack(side="right")

        self.rows_var = tk.StringVar(value="filas: 0")
        tk.Label(top, textvariable=self.rows_var, font=FONT_BODY,
                 bg=BG, fg=FG, padx=16).pack(side="right")

        # ── Contenedor de tabla ──────────────────────────────────
        self.frame = tk.Frame(root, bg=BG)
        self.frame.pack(fill="both", expand=True, padx=12, pady=(0, 10))

        self._build_placeholder()

        # ── Estilos ttk ─────────────────────────────────────────
        style = ttk.Style()
        style.theme_use("clam")
        style.configure("Sim.Treeview",
            background=ROW_ODD, foreground=FG,
            fieldbackground=ROW_ODD, rowheight=22,
            font=FONT_BODY, borderwidth=0)
        style.configure("Sim.Treeview.Heading",
            background=HEADER_BG, foreground=FG_HEAD,
            font=FONT_HEAD, relief="flat", borderwidth=1)
        style.map("Sim.Treeview",
            background=[("selected", "#1e4a7a")],
            foreground=[("selected", "#ffffff")])

        self._poll()

    def _build_placeholder(self):
        lbl = tk.Label(self.frame,
            text="Esperando sim_data.csv ...",
            font=("Consolas", 11), bg=BG, fg=YELLOW)
        lbl.pack(expand=True)
        self._placeholder = lbl

    def _build_table(self, columns):
        """Construye el Treeview con las columnas detectadas del CSV."""
        # Limpiar frame
        for w in self.frame.winfo_children():
            w.destroy()

        self.columns = columns

        # Scrollbars
        vsb = ttk.Scrollbar(self.frame, orient="vertical")
        hsb = ttk.Scrollbar(self.frame, orient="horizontal")

        self.tree = ttk.Treeview(
            self.frame,
            columns=columns,
            show="headings",
            style="Sim.Treeview",
            yscrollcommand=vsb.set,
            xscrollcommand=hsb.set
        )
        vsb.config(command=self.tree.yview)
        hsb.config(command=self.tree.xview)

        # Anchos de columna según prefijo
        def col_width(name):
            n = name.lower()
            if n == "hora_actual":   return 90
            if "llegada" in n:       return 130
            if "fin_serv" in n:      return 100
            if "cant_cola" in n:     return 120
            if "estado" in n:        return 90
            if "desert" in n:        return 120
            if "ocup" in n:          return 110
            return 130

        for col in columns:
            label = col.replace("_", " ")
            self.tree.heading(col, text=label, anchor="center")
            self.tree.column(col, width=col_width(col),
                             anchor="center", minwidth=50, stretch=True)

        self.tree.tag_configure("odd",  background=ROW_ODD)
        self.tree.tag_configure("even", background=ROW_EVEN)

        vsb.pack(side="right",  fill="y")
        hsb.pack(side="bottom", fill="x")
        self.tree.pack(fill="both", expand=True)

    def _load_csv(self):
        """Lee el CSV y devuelve (header, rows) o None si no existe/cambió."""
        if not os.path.exists(CSV_PATH):
            return None
        try:
            mtime = os.path.getmtime(CSV_PATH)
            with open(CSV_PATH, newline="", encoding="utf-8") as f:
                reader = list(csv.reader(f))
            if len(reader) < 1:
                return None
            return mtime, reader[0], reader[1:]
        except Exception:
            return None

    def _poll(self):
        result = self._load_csv()

        if result is None:
            self.status_var.set("⏳ esperando sim_data.csv ...")
        else:
            mtime, header, rows = result

            # Reconstruir tabla si cambiaron las columnas
            if header != self.columns:
                self._build_table(header)

            if mtime != self.last_mtime:
                self.last_mtime = mtime
                self._refresh_rows(rows)
                self.status_var.set("● EN VIVO")
                self.rows_var.set(f"filas: {len(rows)}")

        self.root.after(POLL_MS, self._poll)

    def _refresh_rows(self, rows):
        if self.tree is None:
            return
        # Borrar todo y reinsertar (la tabla puede crecer pero nunca es muy grande)
        self.tree.delete(*self.tree.get_children())
        for i, row in enumerate(rows):
            # Rellenar con vacío si la fila tiene menos columnas
            padded = row + [""] * max(0, len(self.columns) - len(row))
            tag = "even" if i % 2 == 0 else "odd"
            self.tree.insert("", "end", values=padded, tags=(tag,))

        # Auto-scroll al final
        children = self.tree.get_children()
        if children:
            self.tree.see(children[-1])


def main():
    global CSV_PATH
    if len(sys.argv) > 1:
        CSV_PATH = sys.argv[1]

    root = tk.Tk()
    SimGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
