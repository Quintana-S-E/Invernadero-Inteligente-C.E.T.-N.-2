// Create the charts when the web page loads
window.addEventListener('load', onload);

function onload(event) {
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
}

// Create Temperature Chart
function createTemperatureChart(renderToId) {
	var chart = new Highcharts.Chart({
		time:{
			useUTC: false
		},
		chart: {
			renderTo: renderToId,
			type: 'spline'
		},
		series: [
			{
				name: 'Sensor AHT10'
			}
		],
		title: {
			text: undefined
		},
		plotOptions: {
			line: {
				animation: false,
				dataLabels: {
					enabled: true
				}
			}
		},
		xAxis: {
			type: 'datetime',
			dateTimeLabelFormats: { second: '%H:%M:%S' }
		},
		yAxis: {
			title: {
				text: 'Temperatura (°C)'
			}
		},
		credits: {
			enabled: false
		}
	});
	return chart;
}

// Create Humidity Chart
function createHumidityChart(renderToId, AHT10) {
	var chart = new Highcharts.Chart({
		time:{
			useUTC: false
		},
		chart: {
			renderTo: renderToId,
			type: 'spline'
		},
		series: [{
			name: AHT10 ? 'Sensor AHT10' : 'Sensor capacitivo'
		}],
		title: {
			text: undefined
		},
		plotOptions: {
			line: {
				animation: false,
				dataLabels: {
					enabled: true
				}
			},
			series: {
				color: '#50b8b4'
			}
		},
		xAxis: {
			type: 'datetime',
			dateTimeLabelFormats: { second: '%H:%M:%S' }
		},
		yAxis: {
			title: {
				text: 'Humedad (%)'
			}
		},
		credits: {
			enabled: false
		}
	});
	return chart;
}