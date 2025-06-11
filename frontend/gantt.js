function renderGanttChart(ganttData, containerId = "ganttContainer") {
    const container = document.getElementById(containerId);
    container.innerHTML = ""; // Clear previous chart

    const chart = document.createElement("div");
    chart.style.display = "flex";
    chart.style.alignItems = "center";
    chart.style.marginTop = "20px";

    ganttData.forEach((pid, index) => {
        const box = document.createElement("div");
        box.textContent = pid;
        box.style.border = "1px solid #333";
        box.style.padding = "10px 5px";
        box.style.minWidth = "40px";
        box.style.textAlign = "center";
        box.style.backgroundColor = pid === "idle" ? "#ccc" : "#9fc9f3";
        box.style.marginRight = "2px";
        chart.appendChild(box);
    });

    // Time indicators
    const timeRow = document.createElement("div");
    timeRow.style.display = "flex";
    timeRow.style.justifyContent = "space-between";
    timeRow.style.fontSize = "12px";

    for (let i = 0; i <= ganttData.length; i++) {
        const timeBox = document.createElement("div");
        timeBox.textContent = i;
        timeBox.style.minWidth = "40px";
        timeBox.style.textAlign = "left";
        timeBox.style.marginRight = "2px";
        timeRow.appendChild(timeBox);
    }

    container.appendChild(chart);
    container.appendChild(timeRow);
}
