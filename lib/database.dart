import 'package:firebase_database/firebase_database.dart';

class FirebaseDatabaseService {
  final FirebaseDatabase _database = FirebaseDatabase.instance;

  DatabaseReference getReference(String path) {
    // ignore: deprecated_member_use
    return _database.reference().child(path);
  }
}
