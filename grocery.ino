import serial
import time
import dash
from dash import dcc, html
from dash.dependencies import Input, Output
import plotly.graph_objects as go

# Initialize serial communication (Adjust COM port and baud rate accordingly)
ser = serial.Serial('COM3', 9600, timeout=1)
time.sleep(2)  # Allow time for connection

def get_grocery_levels():
    """Retrieve grocery levels from Arduino via Serial."""
    ser.write(b'R')  # Send a request signal
    data = ser.readline().decode().strip()
    if data:
        try:
            levels = list(map(int, data.split(',')))
            return levels if len(levels) == 5 else [0] * 5
        except ValueError:
            return [0] * 5
    return [0] * 5

# Dash App Setup
app = dash.Dash(__name__)

grocery_names = ["Rice", "Wheat", "All Purpose Flour", "Salt", "Sugar"]

app.layout = html.Div([
    html.H1("Grocery Level Indicator"),
    dcc.Interval(id='interval-component', interval=2000, n_intervals=0),
    dcc.Graph(id='level-indicator')
])

@app.callback(
    Output('level-indicator', 'figure'),
    Input('interval-component', 'n_intervals')
)
def update_graph(n):
    levels = get_grocery_levels()
    fig = go.Figure()
    for i, name in enumerate(grocery_names):
        fig.add_trace(go.Bar(y=[name], x=[levels[i]], orientation='h', name=name))
    fig.update_layout(title='Grocery Levels', xaxis=dict(title='Level (in %)'))
    return fig

if __name__ == '__main__':
    app.run_server(debug=True)
