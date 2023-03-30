import 'dart:async';

import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'firebase_options.dart';
import 'package:firebase_messaging/firebase_messaging.dart';
import 'database.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(options: DefaultFirebaseOptions.currentPlatform);
  final fcmToken = await FirebaseMessaging.instance.getToken();
  print(fcmToken);
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'IoT Project',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: const MyHomePage(title: 'MCA Div A : Aayush, Harsh and Kaif'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});
  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  bool isAlert = false;
  @override
  Widget build(BuildContext context) {
    final database = FirebaseDatabaseService();
    DatabaseReference ref = FirebaseDatabase.instance.ref("security/isAlert");
    ref.onValue.listen((DatabaseEvent event) {
      final Object? data = event.snapshot.value;
      setState(() {
        isAlert = data != false ? true : false;
      });
    });

    offAlert() {
      final Map<String, bool> updates = {};
      updates['/security/isAlert'] = false;
      FirebaseDatabase.instance.ref().update(updates);
    }

    return Scaffold(
        appBar: AppBar(
          // Here we take the value from the MyHomePage object that was created by
          // the App.build method, and use it to set our appbar title.
          title: Text(widget.title),
        ),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              const Text(
                  'This is an application to demonstrate laser security alarm.\n\nDeveloped By : Aayush Solanki\nCoordinated By : Harsh Solanki and Kaif',
                  textAlign: TextAlign.center,
                  style: TextStyle(fontWeight: FontWeight.bold, fontSize: 20)),
              const SizedBox(height: 150),
              ElevatedButton(
                  onPressed: offAlert,
                  style: ElevatedButton.styleFrom(
                      primary: isAlert
                          ? Colors.red
                          : Colors.blueAccent, // Change the color here..
                      elevation: 0),
                  child: isAlert
                      ? const Text('Ignore Alert')
                      : const Text('No Alert Found')),
              isAlert
                  ? const Text('Alert! Laser interruption detected',
                      style: TextStyle(
                          color: Colors.red, fontWeight: FontWeight.bold))
                  : const SizedBox(height: 40)
            ],
          ),
        ));
  }
}
