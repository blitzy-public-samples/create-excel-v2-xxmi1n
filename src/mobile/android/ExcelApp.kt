package com.example.excel.mobile.android

import android.app.Application
import androidx.compose.runtime.CompositionLocalProvider
import dagger.hilt.android.HiltAndroidApp
import com.example.excel.di.AppModule
import com.example.excel.data.ExcelRepository
import com.example.excel.utils.LocalExcelRepository
import javax.inject.Inject

@HiltAndroidApp
class ExcelApp : Application() {

    // Inject the ExcelRepository using Hilt
    @Inject
    lateinit var excelRepository: ExcelRepository

    override fun onCreate() {
        super.onCreate()
        setupCompositionLocalProviders()
    }

    // Set up CompositionLocal providers for the app
    private fun setupCompositionLocalProviders() {
        CompositionLocalProvider(
            LocalExcelRepository provides excelRepository
        ) {
            // The rest of the app's UI can now access the ExcelRepository
            // through LocalExcelRepository.current
        }
    }
}

// Human tasks:
// 1. Implement the ExcelRepository interface and its concrete implementation
// 2. Set up the Hilt modules and dependencies in the AppModule
// 3. Create the LocalExcelRepository CompositionLocal in the utils package
// 4. Test the dependency injection and ensure the ExcelRepository is available throughout the app