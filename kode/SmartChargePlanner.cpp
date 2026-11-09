#include <iostream>
#include <iomanip>
#include <limits>
#include <cmath>

using namespace std;

// Menghitung kebutuhan energi dalam kWh.
double hitungEnergi(double kapasitas, double socAwal, double socTarget) {
    return kapasitas * (socTarget - socAwal) / 100.0;
}

// Menghitung lama pengisian dalam menit pada daya konstan.
double hitungDurasi(double energi, double dayaKW) {
    return energi / dayaKW * 60.0;
}

// Penyesuaian berurutan: biaya dasar -> tambahan 10% -> diskon 5%.
double hitungBiaya(double energi, double tarif, bool jamSibuk, bool anggota) {
    double biaya = energi * tarif;
    if (jamSibuk) {
        biaya = biaya * 1.10;
    }
    if (anggota) {
        biaya = biaya * 0.95;
    }
    return biaya;
}

// Meminta bilangan dan mengulangi input bila tidak sesuai batas.
double bacaAngka(int jenis, double minimum, double maksimum,
                 bool eksklusifMinimum, bool eksklusifMaksimum) {
    double nilai;
    bool valid;
    do {
        if (jenis == 1) {
            cout << "Kapasitas baterai (kWh, > 0): ";
        } else if (jenis == 2) {
            cout << "SoC awal (0 sampai kurang dari 100%): ";
        } else if (jenis == 3) {
            cout << "SoC target (lebih tinggi dari awal, maks. 100%): ";
        } else {
            cout << "Anggaran maksimum (Rp, >= 0): ";
        }

        cin >> nilai;
        if (cin.fail()) {
            if (cin.eof()) {
                cout << "\nInput dihentikan.\n";
                return -1.0;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input tidak valid: masukkan angka.\n";
            valid = false;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            valid = isfinite(nilai) &&
                    (eksklusifMinimum ? nilai > minimum : nilai >= minimum) &&
                    (eksklusifMaksimum ? nilai < maksimum : nilai <= maksimum);
            if (!valid) {
                cout << "Input di luar batas yang diizinkan. Silakan ulangi.\n";
            }
        }
    } while (!valid);
    return nilai;
}

// Menu dan opsi jawaban hanya menerima bilangan bulat dalam rentang.
int bacaPilihan(int jenis, int minimum, int maksimum) {
    double input;
    bool valid;
    do {
        if (jenis == 1) {
            cout << "Pilih menu (1-2): ";
        } else if (jenis == 2) {
            cout << "Anggota? (1=Ya, 0=Tidak): ";
        } else if (jenis == 3) {
            cout << "Jam sibuk? (1=Ya, 0=Tidak): ";
        } else {
            cout << "Prioritas (1=Hemat, 2=Cepat): ";
        }

        cin >> input;
        if (cin.fail()) {
            if (cin.eof()) {
                cout << "\nInput dihentikan.\n";
                return -1;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            valid = false;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            valid = isfinite(input) && input >= minimum &&
                    input <= maksimum && floor(input) == input;
        }
        if (!valid) {
            cout << "Pilihan tidak valid. Masukkan bilangan bulat yang tersedia.\n";
        }
    } while (!valid);
    return static_cast<int>(input);
}

// Fungsi void: menampilkan perbandingan dan alasan rekomendasi.
void tampilkanLaporan(double energi, double durasiAC, double biayaAC,
                      double durasiDC, double biayaDC,
                      double anggaran, int prioritas) {
    bool acCukup = biayaAC <= anggaran;
    bool dcCukup = biayaDC <= anggaran;

    cout << "\n========== HASIL SMARTCHARGE PLANNER ==========\n";
    cout << fixed << setprecision(2);
    cout << "Energi dibutuhkan : " << energi << " kWh\n";
    cout << "AC / Standar (7 kW)\n";
    cout << "  Durasi           : " << durasiAC << " menit\n";
    cout << fixed << setprecision(0);
    cout << "  Biaya            : Rp" << biayaAC << "\n";
    cout << "  Status anggaran  : " << (acCukup ? "CUKUP" : "TIDAK CUKUP") << "\n";

    cout << "DC / Cepat (50 kW)\n";
    cout << fixed << setprecision(2);
    cout << "  Durasi           : " << durasiDC << " menit\n";
    cout << fixed << setprecision(0);
    cout << "  Biaya            : Rp" << biayaDC << "\n";
    cout << "  Status anggaran  : " << (dcCukup ? "CUKUP" : "TIDAK CUKUP") << "\n";
    cout << "Anggaran           : Rp" << anggaran << "\n";

    if (acCukup && dcCukup) {
        if (prioritas == 1) {
            cout << "REKOMENDASI: AC / Standar\n";
            cout << "Alasan: kedua opsi cukup anggaran; prioritas Anda hemat biaya.\n";
        } else {
            cout << "REKOMENDASI: DC / Cepat\n";
            cout << "Alasan: kedua opsi cukup anggaran; prioritas Anda waktu tercepat.\n";
        }
    } else if (acCukup) {
        cout << "REKOMENDASI: AC / Standar\n";
        cout << "Alasan: hanya AC sesuai anggaran. Kompromi: pengisian lebih lama.\n";
    } else if (dcCukup) {
        cout << "REKOMENDASI: DC / Cepat\n";
        cout << "Alasan: hanya DC sesuai anggaran. Kompromi: biaya lebih tinggi.\n";
    } else {
        cout << "REKOMENDASI: TIDAK ADA\n";
        cout << "PERINGATAN: anggaran belum mencukupi untuk AC maupun DC.\n";
        cout << "Pertimbangkan menambah anggaran atau menurunkan target SoC.\n";
    }
    cout << "===============================================\n";
}

int main() {
    const double DAYA_AC = 7.0;
    const double DAYA_DC = 50.0;
    const double TARIF_AC = 2400.0;
    const double TARIF_DC = 3200.0;
    int menu;
    int jumlahSimulasi = 0;

    cout << "SMARTCHARGE PLANNER - SIMULASI PENGISIAN EV\n";
    cout << "Catatan: tarif, daya, dan efisiensi hanya asumsi studi kasus.\n";

    do {
        cout << "\n=============== MENU UTAMA ===============\n";
        cout << "1. Simulasi pengisian\n";
        cout << "2. Keluar\n";
        menu = bacaPilihan(1, 1, 2);
        if (menu == -1) return 0;

        if (menu == 1) {
            cout << "\n--- INPUT SIMULASI ---\n";
            double kapasitas = bacaAngka(1, 0.0, numeric_limits<double>::max(), true, false);
            if (kapasitas < 0) return 0;
            double awal = bacaAngka(2, 0.0, 100.0, false, true);
            if (awal < 0) return 0;
            double target = bacaAngka(3, awal, 100.0, true, false);
            if (target < 0) return 0;
            double anggaran = bacaAngka(4, 0.0, numeric_limits<double>::max(), false, false);
            if (anggaran < 0) return 0;
            int anggota = bacaPilihan(2, 0, 1);
            if (anggota == -1) return 0;
            int sibuk = bacaPilihan(3, 0, 1);
            if (sibuk == -1) return 0;
            int prioritas = bacaPilihan(4, 1, 2);
            if (prioritas == -1) return 0;

            double energi = hitungEnergi(kapasitas, awal, target);
            double durasiAC = hitungDurasi(energi, DAYA_AC);
            double durasiDC = hitungDurasi(energi, DAYA_DC);
            double biayaAC = hitungBiaya(energi, TARIF_AC, sibuk == 1, anggota == 1);
            double biayaDC = hitungBiaya(energi, TARIF_DC, sibuk == 1, anggota == 1);

            tampilkanLaporan(energi, durasiAC, biayaAC, durasiDC, biayaDC,
                            anggaran, prioritas);
            jumlahSimulasi++;
            cout << "Simulasi berhasil pada sesi ini: " << jumlahSimulasi << "\n";
        }
    } while (menu != 2);

    cout << "Terima kasih telah menggunakan SmartCharge Planner.\n";
    cout << "Total simulasi valid: " << jumlahSimulasi << "\n";
    return 0;
}
