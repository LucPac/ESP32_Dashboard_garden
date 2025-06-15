// Firebase configuration and initialization
const firebaseConfig = {

};
firebase.initializeApp(firebaseConfig);
firebase.analytics();

// Initialize Firebase refs and DOM elements
const refs = Object.fromEntries(['Location1', 'Location2', 'Location3', 'Location4'].map(loc => [loc, firebase.database().ref(loc)]));
const historyRef = firebase.database().ref('history');
const $ = id => document.getElementById(id);
let currentDistrict = 'Location1';
let charts = {};

// Device history tracking
const deviceLabels = {
  Device1: 'Watering',
  Device2: 'Fan',
  Device3: 'Shade'
};

const addToHistory = (district, deviceKey, state) => {
  const now = new Date();
  const historyItem = {
    district: district,
    device: deviceLabels[deviceKey],
    state: state === '1' ? 'Bật' : 'Tắt',
    timestamp: now.getTime(),
    date: `${now.getDate()}/${now.getMonth() + 1}/${now.getFullYear()}`,
    time: formatTime(now)
  };
  
  historyRef.push(historyItem);
};

// Update history display
const updateHistoryDisplay = () => {
  const historyList = $('history-list');
  if (!historyList) return;

  historyRef.orderByChild('timestamp').limitToLast(100).once('value', snapshot => {
    historyList.innerHTML = ''; // Clear existing items
    
    const items = [];
    snapshot.forEach(childSnapshot => {
      items.push(childSnapshot.val());
    });

    items.reverse().forEach(item => {
      const historyItem = document.createElement('div');
      historyItem.className = 'history-item';
      historyItem.dataset.district = item.district;
      historyItem.dataset.device = item.device;
      historyItem.innerHTML = `
        <div class="history-time">${item.date} ${item.time}</div>
        <div class="history-content">
          <strong>Vườn ${item.district.slice(-1)}:</strong> 
          ${item.device} được ${item.state.toLowerCase()}
        </div>
      `;
      historyList.appendChild(historyItem);
    });

    // Apply current filters after updating
    filterHistory();
  });
};

// Add filter functionality
const filterHistory = () => {
  const districtFilter = $('district-filter').value;
  const deviceFilter = $('device-filter').value;
  const items = document.querySelectorAll('.history-item');

  items.forEach(item => {
    const matchDistrict = districtFilter === 'all' || item.dataset.district === districtFilter;
    const matchDevice = deviceFilter === 'all' || item.dataset.device === deviceFilter;
    item.style.display = matchDistrict && matchDevice ? 'block' : 'none';
  });
};

// Initialize data arrays for time series
let timeLabels = [];
const districtData = {
  Location1: { temp: [], humid: [], light: [] },
  Location2: { temp: [], humid: [], light: [] },
  Location3: { temp: [], humid: [], light: [] },
  Location4: { temp: [], humid: [], light: [] }
};
const MAX_DATA_POINTS = 12;

// Initialize each district's data with zero values
const initializeDistrictData = () => {
  const now = new Date();
  const timeLabel = formatTime(now);
  timeLabels.push(timeLabel);
  
  Object.keys(districtData).forEach(district => {
    districtData[district].temp.push(0);
    districtData[district].humid.push(0);
    districtData[district].light.push(0);
  });
};

// Update display and device states
const updateUI = data => {
  const elements = [
    { key: 'Element1', label: 'Nhiệt độ', unit: '°C', id: 'nhietdo' },
    { key: 'Element2', label: 'Độ ẩm', unit: '%', id: 'doam' },
    { key: 'Element3', label: 'Cường độ ánh sáng', unit: 'lux', id: 'luongmua' }
  ];

  elements.forEach(element => {
    const rawValue = data[element.key] || '0';
    const numValue = parseFloat(rawValue);
    const value = isNaN(numValue) ? 0 : Math.round(numValue);
    $(element.id).innerText = `${value} ${element.unit}`;
  });

  const deviceImages = {
    Watering: { on: 'watering_can_on.png', off: 'watering_can_off.png' },
    Fan: { on: 'fan_on.png', off: 'fan_off.png' },
    Shade: { on: 'shade_on.png', off: 'shade_off.png' }
  };

  [
    ['Watering', 'Device1'],
    ['Fan', 'Device2'],
    ['Shade', 'Device3']
  ].forEach(([id, key]) => {
    const state = parseInt(data[key] || '0');
    const btnOn = $(`btn${id}On`);
    const btnOff = $(`btn${id}Off`);
    btnOn.classList.toggle('active', state === 1);
    btnOn.classList.toggle('inactive', state !== 1);
    btnOff.classList.toggle('active', state !== 1);
    btnOff.classList.toggle('inactive', state === 1);
    $(`img${id}`).src = `image/${state ? deviceImages[id].on : deviceImages[id].off}`;
  });
};

// District selection
const selectDistrict = (district, button) => {
  currentDistrict = district;
  document.querySelectorAll('#banner button').forEach(btn => {
    btn.classList.toggle('active', btn === button);
    btn.classList.toggle('inactive', btn !== button);
  });
  refs[district].once('value', snap => updateUI(snap.val() || { 
    Element1: '0', 
    Element2: '0', 
    Element3: '0', 
    Device1: '0', 
    Device2: '0', 
    Device3: '0' 
  }));
};

// Format time for display
const formatTime = (date) => {
  const hours = date.getHours().toString().padStart(2, '0');
  const minutes = date.getMinutes().toString().padStart(2, '0');
  const seconds = date.getSeconds().toString().padStart(2, '0');
  return `${hours}:${minutes}:${seconds}`;
};

// Check if 5 minutes have passed
const shouldUpdateData = (() => {
  let lastUpdate = new Date();
  return () => {
    const now = new Date();
    const timeDiff = now - lastUpdate;
    if (timeDiff >= 5 * 60 * 1000) { // 5 minutes in milliseconds
      lastUpdate = now;
      return true;
    }
    return false;
  };
})();

// Update real-time clock display
const updateRealTimeClock = () => {
  const now = new Date();
  const timeString = formatTime(now);
  
  // Request next update
  requestAnimationFrame(updateRealTimeClock);
};

// Update time series data
const updateTimeSeriesData = (district, data) => {
  const now = new Date();
  const timeLabel = formatTime(now);
  
  if (timeLabels.length === 0) {
    initializeDistrictData();
  }

  if (timeLabels.length >= MAX_DATA_POINTS) {
    timeLabels.shift();
    Object.values(districtData).forEach(data => {
      data.temp.shift();
      data.humid.shift();
      data.light.shift();
    });
  }

  if (!timeLabels.includes(timeLabel)) {
    timeLabels.push(timeLabel);
    Object.keys(districtData).forEach(d => {
      if (d === district) {
        districtData[d].temp.push(parseFloat(data.Element1 || '0'));
        districtData[d].humid.push(parseFloat(data.Element2 || '0'));
        districtData[d].light.push(parseFloat(data.Element3 || '0'));
      } else {
        const lastIndex = districtData[d].temp.length - 1;
        districtData[d].temp.push(districtData[d].temp[lastIndex] || 0);
        districtData[d].humid.push(districtData[d].humid[lastIndex] || 0);
        districtData[d].light.push(districtData[d].light[lastIndex] || 0);
      }
    });
  } else {
    const lastIndex = timeLabels.length - 1;
    districtData[district].temp[lastIndex] = parseFloat(data.Element1 || '0');
    districtData[district].humid[lastIndex] = parseFloat(data.Element2 || '0');
    districtData[district].light[lastIndex] = parseFloat(data.Element3 || '0');
  }

  updateCharts();
};

// Create and update charts
const createChart = async (config) => {
  showSection(config.sectionId);
  const canvas = $(`${config.chartId}Chart`);
  const errorElement = $(config.errorId);
  
  errorElement.style.display = 'none';
  canvas.style.display = 'block';

  if (charts[config.chartId]) charts[config.chartId].destroy();

  const datasets = Object.entries(districtData).map(([district, data], index) => ({
    label: `Vườn ${district.slice(-1)}`,
    data: config.getData(district),
    borderColor: config.colors.border[index],
    backgroundColor: config.colors.bg[index],
    tension: 0.1,
    fill: false,
    pointRadius: 5,
    pointHoverRadius: 7,
    borderWidth: 2,
    pointStyle: ['circle', 'triangle', 'rect', 'crossRot'][index],
    pointBackgroundColor: config.colors.border[index],
    pointBorderColor: config.colors.border[index]
  }));

  charts[config.chartId] = new Chart(canvas.getContext('2d'), {
    type: 'line',
    data: {
      labels: timeLabels,
      datasets: datasets
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      animation: {
        duration: 0
      },
      interaction: {
        intersect: false,
        mode: 'index'
      },
      scales: {
        y: { 
          beginAtZero: true,
          grid: {
            color: 'rgba(0, 0, 0, 0.1)',
            drawBorder: true,
            lineWidth: 1
          },
          border: {
            color: 'rgba(0, 0, 0, 0.5)',
            width: 1
          },
          ticks: {
            font: { size: 12 },
            padding: 5,
            stepSize: 50
          },
          title: { 
            display: true, 
            text: config.yLabel, 
            font: { size: 14, weight: 'bold' },
            padding: { bottom: 10 }
          }
        },
        x: { 
          grid: {
            color: 'rgba(0, 0, 0, 0.1)',
            drawBorder: true,
            lineWidth: 1
          },
          border: {
            color: 'rgba(0, 0, 0, 0.5)',
            width: 1
          },
          title: { 
            display: true, 
            text: 'Thời gian', 
            font: { size: 14, weight: 'bold' },
            padding: { top: 10 }
          },
          ticks: { 
            font: { size: 12 },
            maxRotation: 45,
            minRotation: 45,
            padding: 5
          }
        }
      },
      plugins: {
        title: { 
          display: true, 
          text: config.title, 
          font: { size: 16, weight: 'bold' },
          padding: { bottom: 20 }
        },
        legend: { 
          display: true,
          position: 'top',
          align: 'center',
          labels: { 
            font: { size: 12 },
            usePointStyle: true,
            pointStyle: 'circle',
            boxWidth: 8,
            padding: 15
          }
        },
        tooltip: {
          enabled: true,
          mode: 'index',
          intersect: false,
          backgroundColor: 'rgba(255, 255, 255, 0.9)',
          titleColor: '#000',
          bodyColor: '#000',
          borderColor: '#ddd',
          borderWidth: 1,
          padding: 10,
          callbacks: {
            label: function(context) {
              const value = context.parsed.y.toFixed(1);
              const unit = config.yLabel.split('(')[1].split(')')[0];
              return `${context.dataset.label}: ${value} ${unit}`;
            }
          }
        }
      }
    }
  });
};

// Update all charts
const updateCharts = () => {
  Object.values(chartConfigs).forEach(config => {
    if (charts[config.chartId]) {
      const chart = charts[config.chartId];
      chart.data.labels = timeLabels;
      chart.data.datasets.forEach((dataset, index) => {
        const district = `Location${index + 1}`;
        dataset.data = [...config.getData(district)];
      });
      chart.update('none');
    }
  });
};

// Chart configurations
const chartConfigs = {
  temperature: {
    sectionId: 'temp-chart',
    chartId: 'temperature',
    title: 'Biểu đồ nhiệt độ theo thời gian',
    yLabel: 'Nhiệt độ (°C)',
    getData: (district) => districtData[district].temp,
    errorId: 'temp-error',
    colors: {
      bg: ['rgba(0, 0, 255, 0.1)', 'rgba(255, 0, 255, 0.1)', 'rgba(255, 0, 0, 0.1)', 'rgba(128, 0, 128, 0.1)'],
      border: ['#0000FF', '#FF00FF', '#FF0000', '#800080']
    }
  },
  humidity: {
    sectionId: 'humidity-chart',
    chartId: 'humidity',
    title: 'Biểu đồ độ ẩm theo thời gian',
    yLabel: 'Độ ẩm (%)',
    getData: (district) => districtData[district].humid,
    errorId: 'humidity-error',
    colors: {
      bg: ['rgba(0, 0, 255, 0.1)', 'rgba(255, 0, 255, 0.1)', 'rgba(255, 0, 0, 0.1)', 'rgba(128, 0, 128, 0.1)'],
      border: ['#0000FF', '#FF00FF', '#FF0000', '#800080']
    }
  },
  light: {
    sectionId: 'light-chart',
    chartId: 'light',
    title: 'Biểu đồ cường độ ánh sáng theo thời gian',
    yLabel: 'Cường độ ánh sáng (lux)',
    getData: (district) => districtData[district].light,
    errorId: 'light-error',
    colors: {
      bg: ['rgba(0, 0, 255, 0.1)', 'rgba(255, 0, 255, 0.1)', 'rgba(255, 0, 0, 0.1)', 'rgba(128, 0, 128, 0.1)'],
      border: ['#0000FF', '#FF00FF', '#FF0000', '#800080']
    }
  }
};

// Initialize districts and set up real-time updates
selectDistrict('Location1', document.querySelector('#banner button'));
Object.entries(refs).forEach(([district, ref]) => {
  // Store previous device states for comparison
  let previousDeviceStates = {
    Device1: null,
    Device2: null,
    Device3: null
  };

  ref.on('value', snap => {
    const data = snap.val() || {};
    if (currentDistrict === district) {
      updateUI(data);
    }
    // Always update time series data regardless of current district
    updateTimeSeriesData(district, data);
    
    // Check for device state changes and update history
    ['Device1', 'Device2', 'Device3'].forEach((deviceKey, index) => {
      const currentState = data[deviceKey];
      // Only add to history if the state has changed and it's not the initial load
      if (previousDeviceStates[deviceKey] !== null && currentState !== previousDeviceStates[deviceKey]) {
        addToHistory(district, deviceKey, currentState);
        
        // If history section is currently visible, update it
        if ($('history-section').classList.contains('active')) {
          updateHistoryDisplay();
        }
      }
      previousDeviceStates[deviceKey] = currentState;
    });

    // Update device states if this is the current district
    if (currentDistrict === district) {
      // Update device buttons based on Firebase data
      ['Device1', 'Device2', 'Device3'].forEach((deviceKey, index) => {
        const deviceNames = ['Watering', 'Fan', 'Shade'];
        const deviceName = deviceNames[index];
        const state = parseInt(data[deviceKey] || '0');
        
        const btnOn = $(`btn${deviceName}On`);
        const btnOff = $(`btn${deviceName}Off`);
        if (btnOn && btnOff) {
          btnOn.classList.toggle('active', state === 1);
          btnOn.classList.toggle('inactive', state !== 1);
          btnOff.classList.toggle('active', state !== 1);
          btnOff.classList.toggle('inactive', state === 1);
          
          // Update device image
          const imgElement = $(`img${deviceName}`);
          if (imgElement) {
            const deviceImages = {
              Watering: { on: 'watering_can_on.png', off: 'watering_can_off.png' },
              Fan: { on: 'fan_on.png', off: 'fan_off.png' },
              Shade: { on: 'shade_on.png', off: 'shade_off.png' }
            };
            imgElement.src = `image/${state ? deviceImages[deviceName].on : deviceImages[deviceName].off}`;
          }
        }
      });
    }
  });
});

// Start real-time clock updates
requestAnimationFrame(updateRealTimeClock);

// Update data every 5 minutes
setInterval(() => {
  Object.entries(refs).forEach(([district, ref]) => {
    ref.once('value', snap => {
      const data = snap.val() || {};
      updateTimeSeriesData(district, data);
    });
  });
}, 5 * 60 * 1000);

// Device controls
[
  ['Watering', 'Device1'],
  ['Fan', 'Device2'],
  ['Shade', 'Device3']
].forEach(([id, key]) => {
  ['On', 'Off'].forEach(state => {
    $(`btn${id}${state}`).addEventListener('click', () => {
      const value = state === 'On' ? '1' : '0';
      refs[currentDistrict].update({ [key]: value });
      addToHistory(currentDistrict, key, value);
    });
  });
});

// Show history section
const showHistory = () => {
  document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
  $('history-section').classList.add('active');
  $('banner').classList.add('hidden');
  updateHistoryDisplay();
  toggleSidenav();
  
  // Scroll to top of history section
  $('history-section').scrollTop = 0;
  
  // Ensure main content area is scrolled to top
  $('main').scrollTop = 0;
};

// Clock update
const updateClock = () => {
  const now = new Date();
  const pad = n => n.toString().padStart(2, '0');
  $('footer-time').innerText = `${pad(now.getHours())}:${pad(now.getMinutes())}:${pad(now.getSeconds())}`;
  $('footer-date').innerText = `${pad(now.getDate())}/${pad(now.getMonth() + 1)}/${now.getFullYear()}`;
  setTimeout(updateClock, 1000);
};
updateClock();

// Sidenav toggle
const toggleSidenav = () => {
  $('sidenav').classList.toggle('open');
  document.querySelector('.hamburger').classList.toggle('open');
};

document.addEventListener('click', e => {
  if (!e.target.closest('#sidenav') && !e.target.closest('.hamburger')) {
    $('sidenav').classList.remove('open');
    document.querySelector('.hamburger').classList.remove('open');
  }
});

// Section display
const showSection = sectionId => {
  if (sectionId === 'history') {
    showHistory();
    return;
  }
  document.querySelectorAll('.section').forEach(s => s.classList.toggle('active', s.id === sectionId));
  $('banner').classList.toggle('hidden', sectionId !== 'home');
  toggleSidenav();
};

// Chart display functions
const showTemperatureChart = () => createChart(chartConfigs.temperature);
const showHumidityChart = () => createChart(chartConfigs.humidity);
const showLightChart = () => createChart(chartConfigs.light);

// Initialize data when the script starts
initializeDistrictData(); 