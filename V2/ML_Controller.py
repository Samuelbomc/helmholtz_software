from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import PolynomialFeatures
from sklearn.pipeline import Pipeline
from sklearn.ensemble import RandomForestRegressor
from sklearn.neural_network import MLPRegressor
from sklearn.preprocessing import StandardScaler
import numpy as np
import pandas as pd
import joblib

class ML:
    "RandomForest, Linear Regression, Neural Network and Polynomial Regression"
    def __init__(self, model_name, data="Training_data.csv"):
        self.model_name = model_name
        self.data_path = data
        
        if model_name == "LR":
            self.model = LinearRegression()
        elif model_name == "PR":
            self.model = Pipeline([
                ("poly", PolynomialFeatures(degree=2, include_bias=False)),
                ("linreg", LinearRegression())
            ])
        elif model_name == "RF":
            self.model = RandomForestRegressor(
                n_estimators=200,   # número de árboles
                max_depth=None,     # sin límite de profundidad
                random_state=42,
                n_jobs=-1
            )
        elif model_name == "NN":
            self.model = Pipeline([
                ("scaler", StandardScaler()),  # escalar datos para ayudar al entrenamiento
                ("nn", MLPRegressor(
                    hidden_layer_sizes=(64, 64),  # dos capas ocultas de 64 neuronas cada una
                    activation="relu",            # función de activación
                    solver="adam",                # optimizador
                    max_iter=1000,                # número máximo de iteraciones
                    random_state=42
                ))
            ])
        else:
            raise Exception("Model not supported")
    
        df = pd.read_csv("Training_data.csv")

        X = df[["Bx (µT)", "By (µT)", "Bz (µT)"]]
        y = df[["Vx", "Vy", "Vz"]]

        X_train, self.X_test, y_train, self.y_test = train_test_split(X, y, test_size=0.2, random_state=42)

        self.model.fit(X_train, y_train)
        
    def get_score(self):
        score = self.model.score(self.X_test, self.y_test)
        print("R²:", score)

    def get_prediction(self, MagX, MagY, MagZ):
        campo = pd.DataFrame([[MagX, MagY, MagZ]], columns=["Bx (µT)", "By (µT)", "Bz (µT)"])
        predict = self.model.predict(campo)
        return predict[0]

    def save_model(self, filename):
        joblib.dump(self.model, filename)

    def load_model(self, filename):
        self.model = joblib.load(filename)