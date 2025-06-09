const express = require("express");
const fs = require("fs");
const path = require("path");
const { exec } = require("child_process");
const cors = require("cors");

const app = express();
const PORT = 3000;

const frontendDir = path.join(__dirname, "..", "frontend");
const backendDir = path.join(__dirname, "..", "backend");
const dataDir = path.join(__dirname, "..", "data");

app.use(cors());
app.use(express.json());

// Serve frontend files
app.use(express.static(frontendDir));

// API to run simulation
app.post("/simulate", (req, res) => {
  const { algorithm, input } = req.body;

  const execPath = path.join(backendDir, algorithm);
  if (!fs.existsSync(execPath)) {
    return res.status(400).json({ error: `Algorithm binary not found: ${algorithm}` });
  }

  const inputPath = path.join(dataDir, "input.json");
  fs.writeFileSync(inputPath, JSON.stringify(input, null, 2));

  exec(execPath, (error, stdout, stderr) => {
    if (error) {
      console.error(stderr);
      return res.status(500).json({ error: "Execution failed" });
    }

    const outputPath = path.join(dataDir, "output.json");
    if (!fs.existsSync(outputPath)) {
      return res.status(500).json({ error: "Output not found" });
    }

    const outputData = JSON.parse(fs.readFileSync(outputPath));
    res.json(outputData);
  });
});

// Start server
app.listen(PORT, () => {
  console.log(`✅ Server running at http://localhost:${PORT}`);
});
