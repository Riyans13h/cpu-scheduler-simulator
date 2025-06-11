function addProcess() {
  const row = document.createElement("tr");
  row.innerHTML = `
    <td><input name="pid" value="P${Date.now() % 1000}"/></td>
    <td><input name="arrival" type="number" value="0"/></td>
    <td><input name="burst" type="number" value="5"/></td>
    <td><input name="priority" type="number" value="1"/></td>
  `;
  document.querySelector("#processTable tbody").appendChild(row);
}

document.getElementById("schedulerForm").addEventListener("submit", async function (e) {
  e.preventDefault();

  const algorithm = document.getElementById("algorithm").value;
  const quantum = parseInt(document.getElementById("quantum").value);
  const rows = document.querySelectorAll("#processTable tbody tr");

  const processes = Array.from(rows).map(row => {
    const inputs = row.querySelectorAll("input");
    return {
      pid: inputs[0].value,
      arrival: parseInt(inputs[1].value),
      burst: parseInt(inputs[2].value),
      priority: parseInt(inputs[3].value)
    };
  });

  const input = {
    time_quantum: quantum,
    processes
  };

  const response = await fetch(`http://localhost:3000/simulate`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ algorithm, input })
  });

  const output = await response.json();
  renderOutput(output);
});

function renderOutput(output) {
  document.getElementById("outputSection").style.display = "block";

  // Render Gantt
  const gantt = document.getElementById("ganttChart");
  gantt.innerHTML = "";
  output.gantt_chart.forEach(pid => {
    const span = document.createElement("span");
    span.textContent = pid;
    gantt.appendChild(span);
  });

  // Render Table
  const tbody = document.querySelector("#resultsTable tbody");
  tbody.innerHTML = "";
  output.results.forEach(p => {
    const row = document.createElement("tr");
    row.innerHTML = `
      <td>${p.pid}</td>
      <td>${p.arrival}</td>
      <td>${p.burst}</td>
      <td>${p.completion}</td>
      <td>${p.turnaround}</td>
      <td>${p.waiting}</td>
    `;
    tbody.appendChild(row);
  });

  document.getElementById("avgWaiting").textContent = output.average_waiting.toFixed(2);
  document.getElementById("avgTurnaround").textContent = output.average_turnaround.toFixed(2);
}