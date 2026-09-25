import { initializeApp } from
    "https://www.gstatic.com/firebasejs/12.0.0/firebase-app.js";

import {
    getDatabase,
    ref,
    set,
    onValue
} from
    "https://www.gstatic.com/firebasejs/12.0.0/firebase-database.js";


// =============================
// CONFIGURAÇÃO DO FIREBASE
// =============================

const firebaseConfig = {
  apiKey: "AIzaSyAJDXJGxqkrp78tWbO6ThrrzQ_QVIM7MYI",
  authDomain: "projeto-voz-esp32.firebaseapp.com",
  databaseURL: "https://projeto-voz-esp32-default-rtdb.firebaseio.com",
  projectId: "projeto-voz-esp32",
  storageBucket: "projeto-voz-esp32.firebasestorage.app",
  messagingSenderId: "98850407941",
  appId: "1:98850407941:web:fbe0819b848f0005d5be6c"
};


// Inicializa Firebase
const app = initializeApp(firebaseConfig);

// Inicializa Realtime Database
const db = getDatabase(app);


// Referência para /comando
const comandoRef = ref(db, "dispositivos/esp32/rele");


// =============================
// ELEMENTOS DA PÁGINA
// =============================

const botao = document.getElementById("btnVoz");
const status = document.getElementById("status");
const comando = document.getElementById("comando");
const estado = document.getElementById("estado");


// =============================
// RECONHECIMENTO DE VOZ
// =============================

const SpeechRecognition =
    window.SpeechRecognition ||
    window.webkitSpeechRecognition;

if (!SpeechRecognition) {

    status.textContent =
        "Este navegador não suporta reconhecimento de voz.";

} else {

    const recognition = new SpeechRecognition();

    recognition.lang = "pt-BR";
    recognition.continuous = false;
    recognition.interimResults = false;
    recognition.maxAlternatives = 1;


    // Quando o botão for pressionado
    botao.addEventListener("click", () => {

        recognition.start();

        status.textContent = "Escutando...";

    });


    // Quando reconhecer alguma coisa
    recognition.addEventListener("result", async (event) => {

        const texto =
            event.results[0][0].transcript
                .toLowerCase()
                .trim();

        console.log("Reconhecido:", texto);

        comando.textContent = texto;


        // =========================
        // COMANDO LIGA
        // =========================

        if (texto.includes("liga")) {

            await set(comandoRef, true);

            status.textContent =
                "Comando enviado: LIGA";

        }


        // =========================
        // COMANDO DESLIGA
        // =========================

        else if (texto.includes("desliga")) {

            await set(comandoRef, false);

            status.textContent =
                "Comando enviado: DESLIGA";

        }


        else {

            status.textContent =
                "Comando não reconhecido.";

        }

    });


    recognition.addEventListener("start", () => {

        status.textContent =
            "Ouvindo...";

    });


    recognition.addEventListener("end", () => {

        console.log("Reconhecimento finalizado.");

    });


    recognition.addEventListener("error", (event) => {

        console.error("Erro:", event.error);

        status.textContent =
            "Erro no reconhecimento de voz.";

    });

}


// =============================
// MONITORAR O FIREBASE
// =============================
//
// Isso também permite que a página
// saiba quando o estado mudou.
//

onValue(comandoRef, (snapshot) => {

    const valor = snapshot.val();

    console.log("Firebase:", valor);

    estado.textContent =
        valor ? "LIGADO" : "DESLIGADO";

});