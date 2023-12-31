// convert epochtime to JavaScripte Date object
function epochToJsDate(epochTime) {
	return new Date(epochTime * 1000);
}

// convert time to human-readable format YYYY/MM/DD HH:MM:SS
function epochToDateTime(epochTime) {
	var epochDate = new Date(epochToJsDate(epochTime));
	var dateTime = ("00" + epochDate.getDate()).slice(-2) + "/" +
		("00" + (epochDate.getMonth() + 1)).slice(-2) + "/" +
		epochDate.getFullYear() + " " +
		("00" + epochDate.getHours()).slice(-2) + ":" +
		("00" + epochDate.getMinutes()).slice(-2) + ":" +
		("00" + epochDate.getSeconds()).slice(-2);

	return dateTime;
}

// function to plot values on charts
function plotValues(chart, timestamp, value) {
	var x = epochToJsDate(timestamp).getTime();
	var y = Number(value);
	if (chart.series[0].data.length > 40) {
		chart.series[0].addPoint([x, y], true, true, true);
	} else {
		chart.series[0].addPoint([x, y], true, false, true);
	}
}

// DOM elements
const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector('#authentication-bar');
const deleteButtonElement = document.getElementById('delete-button');
const deleteModalElement = document.getElementById('delete-modal');
const deleteDataFormElement = document.querySelector('#delete-data-form');
const viewDataButtonElement = document.getElementById('view-data-button');
const hideDataButtonElement = document.getElementById('hide-data-button');
const tableContainerElement = document.querySelector('#table-container');
const chartsRangeInputElement = document.getElementById('charts-range');
const loadDataButtonElement = document.getElementById('load-data');
const cardsCheckboxElement = document.querySelector('input[name=cards-checkbox]');
const gaugesCheckboxElement = document.querySelector('input[name=gauges-checkbox]');
const chartsCheckboxElement = document.querySelector('input[name=charts-checkbox]');

// DOM elements for sensor readings
const cardsReadingsElement = document.querySelector("#cards-div");
const gaugesReadingsElement = document.querySelector("#gauges-div");
const chartsDivElement = document.querySelector('#charts-div');

const riegoElemento = document.getElementById("riego");
const calefaElemento = document.getElementById("calefa");
const ventElemento = document.getElementById("vent");
const updateElement = document.getElementById("lastUpdate");

const tiempoElemento = document.getElementById("tiempo");
//const las demás variables 

// MANAGE LOGIN/LOGOUT UI
const setupUI = (user) => {
	if (user) {
		//toggle UI elements
		loginElement.style.display = 'none';
		contentElement.style.display = 'block';
		authBarElement.style.display = 'block';
		userDetailsElement.style.display = 'block';
		userDetailsElement.innerHTML = user.email;

		// get user UID to get data from database
		var uid = user.uid;
		console.log(uid);

		// Database paths (with user UID)
		var dbPath = 'Invernadero/lecturas';
		var chartPath = 'Invernadero/graficos/rango';

		// Database references
		var dbRef = firebase.database().ref(dbPath);
		var chartRef = firebase.database().ref(chartPath);

		// CHARTS
		// Number of readings to plot on charts
		var chartRange = 0;
		// Get number of readings to plot saved on database (runs when the page first loads and whenever there's a change in the database)
		chartRef.on('value', snapshot => {
			chartRange = Number(snapshot.val());
			console.log(chartRange);
			// Delete all data from charts to update with new values when a new range is selected
			chartTs.destroy();
			chartTm.destroy();
			chartTi.destroy();
			chartTg.destroy();
			chartHAs.destroy();
			chartHAm.destroy();
			chartHAi.destroy();
			chartHS1.destroy();
			chartHS2.destroy();
			//chartRie.destroy();
			//chartCal.destroy();
			//chartVent.destroy();
			// Render new charts to display new range of data
			chartTs = createTemperatureChart('chart-Ts');
			chartTm = createTemperatureChart('chart-Tm');
			chartTi = createTemperatureChart('chart-Ti');
			chartTg = createTemperatureChart('chart-Tg');
			chartHAs = createHumidityChart('chart-HAs', true);
			chartHAm = createHumidityChart('chart-HAm', true);
			chartHAi = createHumidityChart('chart-HAi', true);
			chartHS1 = createHumidityChart('chart-HS1', false);
			chartHS2 = createHumidityChart('chart-HS2', false);
			//chartRie = createTemperatureChart('chart-Ts');
			//chartCal = createTemperatureChart('chart-Ts');
			//chartVent = createTemperatureChart('chart-Ts');

			// Update the charts with the new range
			// Get the latest readings and plot them on charts (the number of plotted readings corresponds to the chartRange value)
			dbRef.orderByKey().limitToLast(chartRange).on('child_added', snapshot => {
				var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, pressure: 1008.48, timestamp:1641317355}
				// Save values on variables
				var Ts = jsonData.Ts;
				var Tm = jsonData.Tm;
				var Ti = jsonData.Ti;
				var Tg = jsonData.Tg;
				var HAs = jsonData.HAs;
				var HAm = jsonData.HAm;
				var HAi = jsonData.HAi;
				var HS1 = jsonData.HS1;
				var HS2 = jsonData.HS2;
				var timestamp = jsonData.unix;
				// Plot the values on the charts
				plotValues(chartTs, timestamp, Ts);
				plotValues(chartTm, timestamp, Tm);
				plotValues(chartTi, timestamp, Ti);
				plotValues(chartTg, timestamp, Tg);
				plotValues(chartHAs, timestamp, HAs);
				plotValues(chartHAm, timestamp, HAm);
				plotValues(chartHAi, timestamp, HAi);
				plotValues(chartHS1, timestamp, HS1);
				plotValues(chartHS2, timestamp, HS2);
			});
		});

		// Update database with new range (input field)
		chartsRangeInputElement.onchange = () => {
			chartRef.set(chartsRangeInputElement.value);
		};

		//CHECKBOXES
		// Checbox (cards for sensor readings)
		cardsCheckboxElement.addEventListener('change', (e) => {
			if (cardsCheckboxElement.checked) {
				cardsReadingsElement.style.display = 'block';
			}
			else {
				cardsReadingsElement.style.display = 'none';
			}
		});
		// Checbox (gauges for sensor readings)
		gaugesCheckboxElement.addEventListener('change', (e) => {
			if (gaugesCheckboxElement.checked) {
				gaugesReadingsElement.style.display = 'block';
			}
			else {
				gaugesReadingsElement.style.display = 'none';
			}
		});
		// Checbox (charta for sensor readings)
		chartsCheckboxElement.addEventListener('change', (e) => {
			if (chartsCheckboxElement.checked) {
				chartsDivElement.style.display = 'block';
			}
			else {
				chartsDivElement.style.display = 'none';
			}
		});

		// CARDS
		// Get the latest readings and display on cards
		dbRef.orderByKey().limitToLast(1).on('child_added', snapshot => {
			var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, pressure: 1008.48, timestamp:1641317355}
			var tiempo = jsonData.T;
			var riego = jsonData.RIE;
			var calefa = jsonData.CAL;
			var vent = jsonData.VENT;
			var timestamp = jsonData.unix;
			// Update DOM elements
			tiempoElemento.innerHTML = tiempo;
			riegoElemento.innerHTML = riego ? 'encendido' : 'apagado';
			calefaElemento.innerHTML = calefa ? 'encendida' : 'apagada';
			ventElemento.innerHTML = vent ? 'abierta' : 'cerrada';
			updateElement.innerHTML = epochToDateTime(timestamp);
		});

		// GAUGES
		// Get the latest readings and display on gauges
		dbRef.orderByKey().limitToLast(1).on('child_added', snapshot => {
			var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, pressure: 1008.48, timestamp:1641317355}
			//var temp_int_high = jsonData.Ts;
			var temp_int_mid = jsonData.Tm;
			//var temp_int_low = jsonData.Ti;
			//var temp_geo = jsonData.Tg(C);

			//var hum_int_high = jsonData.HAs;
			//var hum_int_mid = jsonData.HAm;
			//var hum_int_low = jsonData.HAi;
			var hum_soil1 = jsonData.HS1;
			//var hum_soil2 = jsonData.HS2;

			var timestamp = jsonData.unix;
			// Update DOM elements
			//var gaugeT1 = createTemperatureGauge('reloj-temperatura-inth');
			var gaugeT2 = createTemperatureGauge('reloj-temperatura-intm');
			//var gaugeT3 = createTemperatureGauge('reloj-temperatura-intl');
			//var gaugeT4 = createTemperatureGauge('reloj-temperatura-geo');
			//var gaugeH1 = createHumidityGauge('reloj-humedad-inth');
			//var gaugeH2 = createHumidityGauge('reloj-humedad-intm');
			//var gaugeH3 = createHumidityGauge('reloj-humedad-intl');
			var gaugeH4 = createHumidityGauge('reloj-humedad-soil1');
			//var gaugeH5 = createHumidityGauge('reloj-humedad-soil2');
			//gaugeT1.draw();	gaugeT1.value = temp_int_high;
			gaugeT2.draw();	gaugeT2.value = temp_int_mid;
			//gaugeT3.draw();	gaugeT3.value = temp_int_low;
			//gaugeT4.draw();	gaugeT4.value = temp_geo;

			//gaugeH1.draw();	gaugeH1.value = hum_int_high;
			//gaugeH2.draw();	gaugeH2.value = hum_int_mid;
			//gaugeH3.draw();	gaugeH3.value = hum_int_low;
			gaugeH4.draw();	gaugeH4.value = hum_soil1;
			//gaugeH5.draw();	gaugeH5.value = hum_soil2;
			updateElement.innerHTML = epochToDateTime(timestamp);
		});

		// DELETE DATA
		// Add event listener to open modal when click on "Delete Data" button
		deleteButtonElement.addEventListener('click', e => {
			console.log("Remove data");
			e.preventDefault;
			deleteModalElement.style.display = "block";
		});

		// Add event listener when delete form is submited
		deleteDataFormElement.addEventListener('submit', (e) => {
			// delete data (readings)
			dbRef.remove();
		});

		// TABLE
		var lastReadingTimestamp; //saves last timestamp displayed on the table
		// Function that creates the table with the first 100 readings
		function createTable() {
			// append all data to the table
			var firstRun = true;
			dbRef.orderByKey().limitToLast(100).on('child_added', function (snapshot) {
				if (snapshot.exists()) {
					var jsonData = snapshot.toJSON();
					console.log(jsonData);
					var tiempo = jsonData.T;
					var Ts = jsonData.Ts;
					var Tm = jsonData.Tm;
					var Ti = jsonData.Ti;
					var Tg = jsonData.Tg;
					var HAs = jsonData.HAs;
					var HAm = jsonData.HAm;
					var HAi = jsonData.HAi;
					var HS1 = jsonData.HS1;
					var HS2 = jsonData.HS2;
					var modo_riego;
					switch (jsonData.mRIE) {
					case 0:
						modo_riego = 'auto.';
						break;
					case 1:
						modo_riego = 'deshab.';
						break;
					case 2:
						modo_riego = 'forzado';
						break;
					case 3:
						modo_riego = 'temporiz.';
					}
					var modo_calefa;
					switch (jsonData.mCAL) {
					case 0:
						modo_calefa = 'auto.';
						break;
					case 1:
						modo_calefa = 'deshab.';
						break;
					case 2:
						modo_calefa = 'forzada';
						break;
					case 3:
						modo_calefa = 'temporiz.';
					}
					var modo_vent;
					switch (jsonData.mVEN) {
					case 0:
						modo_vent = 'auto.';
						break;
					case 1:
						modo_vent = 'deshab.';
						break;
					case 2:
						modo_vent = 'forzada';
						break;
					case 3:
						modo_vent = 'temporiz.';
					}
					var riego = jsonData.RIE ? 'encendido' : 'apagado';
					var calefa = jsonData.CAL ? 'encendida' : 'apagada';
					var vent = jsonData.VENT ? 'abierta' : 'cerrada';
					var timestamp = jsonData.unix;
					var content = '';
					content += '<tr>';
					content += '<td>' + timestamp + '</td>';
					content += '<td>' + tiempo + '</td>';
					content += '<td>' + Ts + '</td>';
					content += '<td>' + Tm + '</td>';
					content += '<td>' + Ti + '</td>';
					content += '<td>' + Tg + '</td>';
					content += '<td>' + HAs + '</td>';
					content += '<td>' + HAm + '</td>';
					content += '<td>' + HAi + '</td>';
					content += '<td>' + HS1 + '</td>';
					content += '<td>' + HS2 + '</td>';
					content += '<td>' + riego + '</td>';
					content += '<td>' + modo_riego + '</td>';
					content += '<td>' + calefa + '</td>';
					content += '<td>' + modo_calefa + '</td>';
					content += '<td>' + vent + '</td>';
					content += '<td>' + modo_vent + '</td>';
					content += '</tr>';
					$('#tbody').prepend(content);
					// Save lastReadingTimestamp --> corresponds to the first timestamp on the returned snapshot data
					if (firstRun) {
						lastReadingTimestamp = timestamp;
						firstRun = false;
						console.log(lastReadingTimestamp);
					}
				}
			});
		};

		// append readings to table (after pressing More results... button)
		function appendToTable() {
			var dataList = []; // saves list of readings returned by the snapshot (oldest-->newest)
			var reversedList = []; // the same as previous, but reversed (newest--> oldest)
			console.log("APEND");
			dbRef.orderByKey().limitToLast(100).endAt(lastReadingTimestamp).once('value', function (snapshot) {
				// convert the snapshot to JSON
				if (snapshot.exists()) {
					snapshot.forEach(element => {
						var jsonData = element.toJSON();
						dataList.push(jsonData); // create a list with all data
					});
					lastReadingTimestamp = dataList[0].timestamp; //oldest timestamp corresponds to the first on the list (oldest --> newest)
					reversedList = dataList.reverse(); // reverse the order of the list (newest data --> oldest data)

					var firstTime = true;
					// loop through all elements of the list and append to table (newest elements first)
					reversedList.forEach(element => {
						if (firstTime) { // ignore first reading (it's already on the table from the previous query)
							firstTime = false;
						}
						else {
							var tiempo = element.T;
							var Ts = element.Ts;
							var Tm = element.Tm;
							var Ti = element.Ti;
							var Tg = element.Tg;
							var HAs = element.HAs;
							var HAm = element.HAm;
							var HAi = element.HAi;
							var HS1 = element.HS1;
							var HS2 = element.HS2;
							var modo_riego;
							switch (element.mRIE) {
							case 0:
								modo_riego = 'auto';
								break;
							case 1:
								modo_riego = 'deshab.';
								break;
							case 2:
								modo_riego = 'forzado';
								break;
							case 3:
								modo_riego = 'temporiz.';
							}
							var modo_calefa;
							switch (element.mCAL) {
							case 0:
								modo_calefa = 'auto';
								break;
							case 1:
								modo_calefa = 'deshab.';
								break;
							case 2:
								modo_calefa = 'forzada';
								break;
							case 3:
								modo_calefa = 'temporiz.';
							}
							var modo_vent;
							switch (element.mVEN) {
							case 0:
								modo_vent = 'auto';
								break;
							case 1:
								modo_vent = 'deshab.';
								break;
							case 2:
								modo_vent = 'forzada';
								break;
							case 3:
								modo_vent = 'temporiz.';
							}
							var riego = element.RIE ? 'encendido' : 'apagado';
							var calefa = element.CAL ? 'encendida' : 'apagada';
							var vent = element.VENT ? 'abierta' : 'cerrada';
							var timestamp = element.unix;
							var content = '';
							content += '<tr>';
							content += '<td>' + timestamp + '</td>';
							content += '<td>' + tiempo + '</td>';
							content += '<td>' + Ts + '</td>';
							content += '<td>' + Tm + '</td>';
							content += '<td>' + Ti + '</td>';
							content += '<td>' + Tg + '</td>';
							content += '<td>' + HAs + '</td>';
							content += '<td>' + HAm + '</td>';
							content += '<td>' + HAi + '</td>';
							content += '<td>' + HS1 + '</td>';
							content += '<td>' + HS2 + '</td>';
							content += '<td>' + riego + '</td>';
							content += '<td>' + modo_riego + '</td>';
							content += '<td>' + calefa + '</td>';
							content += '<td>' + modo_calefa + '</td>';
							content += '<td>' + vent + '</td>';
							content += '<td>' + modo_vent + '</td>';
							content += '</tr>';
							$('#tbody').append(content);
						}
					});
				}
			});
		}

		viewDataButtonElement.addEventListener('click', (e) => {
			// Toggle DOM elements
			tableContainerElement.style.display = 'block';
			viewDataButtonElement.style.display = 'none';
			hideDataButtonElement.style.display = 'inline-block';
			loadDataButtonElement.style.display = 'inline-block'
			createTable();
		});

		loadDataButtonElement.addEventListener('click', (e) => {
			appendToTable();
		});

		hideDataButtonElement.addEventListener('click', (e) => {
			tableContainerElement.style.display = 'none';
			viewDataButtonElement.style.display = 'inline-block';
			hideDataButtonElement.style.display = 'none';
		});

		// IF USER IS LOGGED OUT
	} else {
		// toggle UI elements
		loginElement.style.display = 'block';
		authBarElement.style.display = 'none';
		userDetailsElement.style.display = 'none';
		contentElement.style.display = 'none';
	}
}