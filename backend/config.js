// import { initializeApp } from "firebase/app";
// import { getFirestore, collection } from "firebase/firestore";
// import { addDoc } from "firebase/firestore";
const {initializeApp} = require("firebase/app")
const {getFirestore, collection, addDoc, getDocs, query, orderBy, limit, startAt, endAt, where} = require("firebase/firestore")
const admin = require("firebase-admin")
const serviceAccount = require("./taal2-b082a-firebase-adminsdk-ryil6-569e47badb.json")

// firestore
// const firebaseConfig = {
//   apiKey: "AIzaSyDwkuJwJO5mDSbqA2TkX7YigkTbk_jC_OY",
//   authDomain: "taal2-b082a.firebaseapp.com",
//   databaseURL: "https://taal2-b082a-default-rtdb.asia-southeast1.firebasedatabase.app",
//   projectId: "taal2-b082a",
//   storageBucket: "taal2-b082a.appspot.com",
//   messagingSenderId: "160138384841",
//   appId: "1:160138384841:web:d3ee70eb35a058849b0418",
//   measurementId: "G-BYBHHLTLJ5"
// }

// old taal db
const firebaseConfig = {
  apiKey: "AIzaSyAqDP-LNW8dpMiexG4J7LBKZTFFfKkwTzQ",
  authDomain: "taal-d9a33.firebaseapp.com",
  databaseURL: "https://taal-d9a33-default-rtdb.firebaseio.com",
  projectId: "taal-d9a33",
  storageBucket: "taal-d9a33.appspot.com",
  messagingSenderId: "842756921793",
  appId: "1:842756921793:web:cbc3025c9e9011b4ecf740",
  measurementId: "G-XLY1LJ4WQ2"
}

// realtime database
admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: "https://taal2-b082a-default-rtdb.asia-southeast1.firebasedatabase.app"
});

const db = admin.database()

// Initialize Firebase
const app = initializeApp(firebaseConfig)
const firestoreDB = getFirestore(app)
const TaalReadings = collection(firestoreDB,"TaalReadings")
const TaalAccounts = collection(firestoreDB, "TaalAccounts")
module.exports = {TaalReadings, TaalAccounts, db, addDoc, getDocs, query, orderBy, limit, startAt, where}