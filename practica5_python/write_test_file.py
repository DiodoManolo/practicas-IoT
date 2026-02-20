from pathlib import Path

out = Path("test_output.txt")
out.write_text("Esto es una prueba\nLinea 2\nLinea 3\n", encoding="utf-8")

print(f"OK: creado {out.resolve()}")